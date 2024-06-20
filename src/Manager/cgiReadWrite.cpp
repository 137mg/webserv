/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgiReadWrite.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/20 13:55:35 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/20 15:05:10 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::handleCGIOutput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);
    char buffer[80096];
    ssize_t bytesRead;

	std::cout << "cgi is gonna read from " << cgiFd << std::endl;
    // Read from the stdoutFd of the CGI process
    bytesRead = read(cgi.stdoutFd, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
		std::string response = "";

		response = "HTTP/1.1 200 OK \r\n";
		response += "Content-Length: " + std::to_string(std::string(buffer).size()) + "\r\n";
		response += "Connection: keep-alive\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += buffer;
		
		close(cgi.stdoutFd);
		sendResponse(response, cgi.clientFd);
		removeCGIProcess(cgiFd);
    }
    else if (bytesRead == 0)
    {
        // No more data to read, handle completion if needed
        close(cgi.stdoutFd); // Close the stdout pipe
        removeCGIProcess(cgiFd); // Remove CGI process from _cgiProcesses
    }
    else
    {
        // Handle read error
        perror("read from CGI stdout failed");
        close(cgi.stdoutFd); // Close the stdout pipe
        removeCGIProcess(cgiFd); // Remove CGI process from _cgiProcesses
    }
	return;
	std::cout << pollIndex;
}

void	Manager::handleCGIInput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess& cgi = getCGIProcessForFd(cgiFd);

	std::cout << "cgi is gonna write to script " << cgi.clientFd << std::endl;
	ssize_t bytesWritten = write(cgiFd, cgi.cgiRequest.c_str(), cgi.cgiRequest.size());
	if (bytesWritten > 0)
	{
		cgi.cgiRequestSent += bytesWritten;
	}
	clearFdForWriting(cgi.clientFd);
	close(cgiFd);
	return;
	std::cout << pollIndex;
}

bool	Manager::isCGIInputFd(int fd)
{
    for (const auto& cgi : _cgiProcesses)
    {
        if (cgi.stdinFd == fd)
        {
            return true;
        }
    }
    return false;
}

void Manager::removeCGIProcess(int fd)
{
	auto it = std::remove_if(_cgiProcesses.begin(), _cgiProcesses.end(),
							 [fd](const t_CGIProcess &cgi) {
								 return cgi.stdinFd == fd || cgi.stdoutFd == fd || cgi.clientFd == fd;
							 });
	if (it != _cgiProcesses.end())
	{
		_cgiProcesses.erase(it, _cgiProcesses.end());
	}
}

t_CGIProcess& Manager::getCGIProcessForFd(int fd)
{
    for (auto& cgi : _cgiProcesses)
    {
        if (cgi.stdinFd == fd || cgi.stdoutFd == fd || cgi.clientFd == fd)
        {
            return cgi;
        }
    }
	throw std::runtime_error("CGI process not found for given file descriptor");
}
