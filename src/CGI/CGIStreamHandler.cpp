/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/04 15:02:20 by juvan-to      ########   odam.nl         */
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
		cgi.moreDataAvailable = (bytesRead == MESSAGE_BUFFER - 1);
	}
	else if (bytesRead < 0)
	{
		perror("Error read from CGI");
		close(cgi.stdoutFd);
		delFromPollFdsByValue(cgi.stdoutFd);
		removeCGIProcess(cgiFd);
		return;
	}
	else if (bytesRead == 0)
		cgi.moreDataAvailable = false;

	if (cgi.moreDataAvailable)
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

void Manager::handleHangup(int cgiFd)
{
    t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);

    waitpid(cgi.pid, &cgi.status, WNOHANG);
    
    if (WIFEXITED(cgi.status))
    {
        if (WEXITSTATUS(cgi.status) != 0)
        {
			Server server = _serverMap[findPort(cgi.clientFd)][0];
			std::string response = server.buildResponse(server.errorPages[500], server.errorMessages[500]);
			this->_clientErrorResponses[cgi.clientFd] = response;
			markFdForWriting(cgi.clientFd);
            
            close(cgi.stdoutFd);
			delFromPollFdsByValue(cgi.stdoutFd);
			removeCGIProcess(cgiFd);
		}
    }
}


// This writes the request to the CGI script
void	Manager::handleCGIInput(int cgiFd)
{
	t_CGIProcess& cgi = getCGIProcessForFd(cgiFd);
	ssize_t				bytesWritten;
	size_t				bytesToWrite;

	bytesToWrite = cgi.cgiRequest.size() - cgi.cgiRequestWritten;
	bytesWritten = write(cgi.stdinFd, cgi.cgiRequest.c_str() + cgi.cgiRequestWritten, bytesToWrite);
	if (bytesWritten > 0)
		cgi.cgiRequestWritten += bytesWritten;
	else if (bytesWritten < 0)
	{
		perror("Error write to CGI");
        close(cgi.stdinFd);
        delFromPollFdsByValue(cgi.stdinFd);
		removeCGIProcess(cgiFd);
        return;
	}
	else if (bytesWritten)
		return;

	if (cgi.cgiRequestWritten == cgi.cgiRequest.size())
	{
		close(cgi.stdinFd);
		delFromPollFdsByValue(cgi.stdinFd);
	}
	else
        markFdForWriting(cgi.stdinFd);
}
