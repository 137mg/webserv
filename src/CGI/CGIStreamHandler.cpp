/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 15:37:12 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

// This will read the output from thr CGI script and build a response with it
void	Manager::handleCGIOutput(int cgiFd)
{
	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
	char			buffer[MESSAGE_BUFFER];
	ssize_t			bytesRead;

	memset(buffer, '\0', MESSAGE_BUFFER);
	bytesRead = read(cgi.stdoutFd, buffer, MESSAGE_BUFFER - 1);
	if (bytesRead > 0)
	{
		cgi.cgiResponseSize += bytesRead;
		cgi.cgiResponse.append(buffer, bytesRead);
	}
	else if (bytesRead < 0)
	{
		perror("read from CGI stdout failed");
		close(cgi.stdoutFd);
		delFromPollFdsByValue(cgi.stdoutFd);
		removeCGIProcess(cgiFd);
		return;
	}
	// Not finished reading yet, wait for more data
	if (bytesRead == MESSAGE_BUFFER)
		return;
	std::string response = "";
	response = "HTTP/1.1 200 OK \r\n";
	response += "Content-Length: " + std::to_string(cgi.cgiResponseSize) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += cgi.cgiResponse;
	
	this->_clientResponses[cgi.clientFd] = response;
	markFdForWriting(cgi.clientFd);
	
	close(cgi.stdoutFd);
	delFromPollFdsByValue(cgi.stdoutFd);
	removeCGIProcess(cgiFd);
}

void	Manager::handleHangup(int cgiFd)
{
	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
	
	std::cout << "before waiting\n";
	waitpid(cgi.pid, &cgi.status, WNOHANG);
	std::cout << "after waiting\n";
	if (WIFEXITED(cgi.status))
	{
		std::cout << WEXITSTATUS(cgi.status) << std::endl;
		if (WEXITSTATUS(cgi.status) != 0)
		{
			std::cout << "error seen\n";
			exit (1);
		}
	}
}


// This writes the request to the CGI script
void	Manager::handleCGIInput(int cgiFd)
{
	t_CGIProcess& cgi = getCGIProcessForFd(cgiFd);

	cgi.cgiRequestWritten = write(cgi.stdinFd, cgi.cgiRequest.c_str(), cgi.cgiRequest.size());
	
	if (cgi.cgiRequestWritten <= 0)
	{
		close(cgi.stdinFd);
		delFromPollFdsByValue(cgi.stdinFd);
		return;
	}
	markFdForWriting(cgiFd);
	close(cgi.stdinFd);
	delFromPollFdsByValue(cgi.stdinFd);
}
