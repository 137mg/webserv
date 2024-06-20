/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgiReadWrite.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/20 13:55:35 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/20 16:44:52 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::handleCGIOutput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);
    char buffer[80096];
    ssize_t bytesRead;

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
		
		sendResponse(response, cgi.clientFd);
		clearFdForWriting(cgiFd);
    }
	if (bytesRead <= 0)
    {
        if (bytesRead < 0)
        {
            perror("read from CGI stdout failed");
        }
		close(cgi.stdoutFd);
		delFromPollFdsByValue(cgi.stdoutFd);
		removeCGIProcess(cgiFd);
    }
	close(cgi.stdoutFd);
	delFromPollFdsByValue(cgi.stdoutFd);
	removeCGIProcess(cgiFd);
	return;
	std::cout << pollIndex;
}

void	Manager::handleCGIInput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess& cgi = getCGIProcessForFd(cgiFd);

	ssize_t bytesWritten = write(cgi.stdinFd, cgi.cgiRequest.c_str(), cgi.cgiRequest.size());
	if (bytesWritten > 0)
	{
		cgi.cgiRequestSent += bytesWritten;
	}
	markFdForWriting(cgiFd);
	close(cgi.stdinFd);
	delFromPollFdsByValue(cgi.stdinFd);
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

void Manager::delFromPollFdsByValue(int fd)
{
    auto it = std::find_if(this->_pollFdsVector.begin(), this->_pollFdsVector.end(),
                           [fd](const pollfd& pfd) { return pfd.fd == fd; });
    if (it != this->_pollFdsVector.end())
    {
        this->_pollFdsVector.erase(it);
    }
}

