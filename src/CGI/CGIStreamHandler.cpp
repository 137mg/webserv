/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIStreamHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 13:05:14 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/21 17:51:39 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

// This will read the output from thr CGI script and build a response with it
void	Manager::handleCGIOutput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);
    char buffer[MESSAGE_BUFFER];
    ssize_t bytesRead;

    // Read from the stdoutFd of the CGI process
	memset(buffer, '\0', MESSAGE_BUFFER);
    bytesRead = read(cgi.stdoutFd, buffer, MESSAGE_BUFFER - 1);
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

// This writes the request to the CGI script
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
