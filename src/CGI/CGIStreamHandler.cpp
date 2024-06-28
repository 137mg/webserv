/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIStreamHandler.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:05:14 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/28 16:48:01 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"


// This will read the output from thr CGI script and build a response with it
void	Manager::handleCGIOutput(int cgiFd)
{
	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
    char			buffer[MESSAGE_BUFFER2];
    ssize_t			bytesRead;

    bytesRead = read(cgi.stdoutFd, buffer, MESSAGE_BUFFER2);
    if (bytesRead > 0)
    {
		cgi.cgiResponseSize += bytesRead;
		cgi.cgiResponse.append(buffer);
    }
	else if (bytesRead < 0)
	{
        perror("read from CGI stdout failed");
		close(cgi.stdoutFd);
		delFromPollFdsByValue(cgi.stdoutFd);
		removeCGIProcess(cgiFd);
		return;
	}
	if (bytesRead < MESSAGE_BUFFER2)
	{
		cgi.cgiResponse[cgi.cgiResponseSize] = '\0';
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
}

// // This will read the output from thr CGI script and build a response with it
// void	Manager::handleCGIOutput(int cgiFd)
// {
// 	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
//     char			buffer[MESSAGE_BUFFER];
//     ssize_t			bytesRead;

//     // Read from the stdoutFd of the CGI process
// 	memset(buffer, '\0', MESSAGE_BUFFER);
//     bytesRead = read(cgi.stdoutFd, buffer, MESSAGE_BUFFER - 1);
//     if (bytesRead > 0)
//     {
// 		buffer[bytesRead] = '\0';
// 		std::string response = "";

// 		response = "HTTP/1.1 200 OK \r\n";
// 		response += "Content-Length: " + std::to_string(std::string(buffer).size()) + "\r\n";
// 		response += "Connection: keep-alive\r\n";
// 		response += "Content-Type: text/html\r\n\r\n";
// 		response += buffer;
		
// 		this->clientResponses[cgi.clientFd] = response;
// 		markFdForWriting(cgi.clientFd);
//     }
// 	if (bytesRead <= 0)
//     {
//         if (bytesRead < 0)
//         {
//             perror("read from CGI stdout failed");
//         }
// 		close(cgi.stdoutFd);
// 		delFromPollFdsByValue(cgi.stdoutFd);
// 		removeCGIProcess(cgiFd);
// 		return;
//     }
// 	close(cgi.stdoutFd);
// 	delFromPollFdsByValue(cgi.stdoutFd);
// 	removeCGIProcess(cgiFd);
// }

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
