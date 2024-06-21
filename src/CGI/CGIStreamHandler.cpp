/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/21 13:07:01 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::handleCGIOutput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);
    char buffer[MESSAGE_BUFFER];
    ssize_t bytesRead;

    // Read from the stdoutFd of the CGI process
    bytesRead = read(cgi.stdoutFd, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
		buffer[bytesRead] = '\0';
		std::string response = "";

		response = "HTTP/1.1 200 OK \r\n";
		response += "Content-Length: " + std::to_string(std::string(buffer).size()) + "\r\n";
		response += "Connection: keep-alive\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += buffer;
		
		this->clientResponses[cgi.clientFd] = response;
		markFdForWriting(cgi.clientFd);
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
