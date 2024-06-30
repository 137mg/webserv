/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/30 02:38:02 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"


// This will read the output from thr CGI script and build a response with it
void	Manager::handleCGIOutput(int cgiFd)
{
	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
    char			buffer[MESSAGE_BUFFER];
    ssize_t			bytesRead;

    bytesRead = read(cgi.stdoutFd, buffer, MESSAGE_BUFFER);
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
	
	waitpid(cgi.pid, &cgi.status, 0);
	if (WIFEXITED(cgi.status))
	{
		int exitStatus = WEXITSTATUS(cgi.status);
		if (exitStatus != 0)
		{
			this->clientErrorResponses[cgi.clientFd] = cgi.cgiErrorResponse;
			markFdForWriting(cgi.clientFd);
			close(cgi.stdoutFd);
			delFromPollFdsByValue(cgi.stdoutFd);
			removeCGIProcess(cgiFd);
			return;
		}
	}
	
	std::string response = "";

	response = "HTTP/1.1 200 OK \r\n";
	response += "Content-Length: " + std::to_string(cgi.cgiResponseSize) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += cgi.cgiResponse;
	
	this->clientResponses[cgi.clientFd] = response;
	markFdForWriting(cgi.clientFd);
	
	close(cgi.stdoutFd);
	delFromPollFdsByValue(cgi.stdoutFd);
	removeCGIProcess(cgiFd);
}


// This writes the request to the CGI script
void	Manager::handleCGIInput(int cgiFd)
{
	t_CGIProcess& cgi = getCGIProcessForFd(cgiFd);

	ssize_t bytesWritten = write(cgi.stdinFd, cgi.cgiRequest.c_str(), cgi.cgiRequest.size());
	if (bytesWritten > 0)
	{
		cgi.cgiRequestWritten += bytesWritten;
	}
	else if (bytesWritten <= 0)
	{
		close(cgi.stdinFd);
		delFromPollFdsByValue(cgi.stdinFd);
	}
	markFdForWriting(cgiFd);
	close(cgi.stdinFd);
	delFromPollFdsByValue(cgi.stdinFd);
}
