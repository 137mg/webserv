/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 15:37:35 by juvan-to      ########   odam.nl         */
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


std::string	buildResponse(const std::string path, std::string status)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	std::string			fileContents;
	std::string			response;

	responseStream << fileStream.rdbuf();
	fileContents = responseStream.str();

	response = "HTTP/1.1 " + status + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
		response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return (response);
}

void Manager::handleHangup(int cgiFd)
{
    t_CGIProcess &cgi = getCGIProcessForFd(cgiFd);

    waitpid(cgi.pid, &cgi.status, WNOHANG);
    
    if (WIFEXITED(cgi.status))
    {
        if (WEXITSTATUS(cgi.status) != 0)
        {
            // Respond with an error message
			this->_clientResponses[cgi.clientFd] = "CGI script exited with status " + std::to_string(WEXITSTATUS(cgi.status));
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
