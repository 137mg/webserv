/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 17:00:22 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/18 17:38:22 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"

// Handle the connection with a client, reading incoming data and processing requests
bool	ServerManager::handleClientConnection(int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read;

	bytes_read = read(clientFd, buffer, MESSAGE_BUFFER);
	if (bytes_read == 0)
	{
		printTimestamp();
		std::cout << GREEN << "Client socket " << RESET << clientFd << RED << " closed " << RESET << "the connection." << std::endl;
		this->_clientBuffers.erase(clientFd);
		return (false);
	}
	else if (bytes_read < 0)
	{
		std::cerr << RED << BOLD << "Read error " << std::strerror(errno) << RESET << std::endl;
		return (false);
	}
	else
	{
		std::string& clientBuffer = this->_clientBuffers[clientFd];
		clientBuffer.append(buffer, bytes_read);
		if (clientBuffer.size() > MB * 10)
		{
			selectServer(clientBuffer, clientFd);
			this->_clientBuffers.erase(clientFd);
			return (false);
		}
		else if (isRequestComplete(clientBuffer))
		{
			selectServer(clientBuffer, clientFd);
			markFdForWriting(clientFd);
			this->_clientBuffers.erase(clientFd);
		}
	}
	return (true);
}

bool ServerManager::isRequestComplete(const std::string &request_buffer)
{
	size_t	headerEnd;
	size_t	totalExpectedSize;
	size_t	contentLengthPos;

	headerEnd = request_buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (false);
	totalExpectedSize = getRequestSize(request_buffer);
	contentLengthPos = request_buffer.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		if (request_buffer.size() >= totalExpectedSize)
			return (true);
	} 
	else
		return (true);
	return (false);
}

// Returns the size of a request
size_t ServerManager::getRequestSize(std::string request_buffer)
{
	// Check if Content-Length header exists
	size_t contentLengthPos = request_buffer.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		size_t contentLengthEnd = request_buffer.find("\r\n", contentLengthPos);
		size_t contentLength = std::stoi(request_buffer.substr(contentLengthPos + 15, contentLengthEnd - contentLengthPos - 15));
		size_t totalExpectedSize = request_buffer.find("\r\n\r\n") + 4 + contentLength;
		return (totalExpectedSize);
	}
	return (request_buffer.size());
}

void	ServerManager::closeClientConnection(unsigned long i)
{
	printTimestamp();
    std::cout << RED << "Closing " << RESET << "client socket " << RESET << _pollFdsVector[i].fd << std::endl;
    close(_pollFdsVector[i].fd);
    delFromPollFds(i);
}

void	ServerManager::handleCGIOutput(int cgiFd, size_t pollIndex)
{
	t_CGIProcess	&cgi = getCGIProcessForFd(cgiFd);
	std::string		httpResponse = "";
    char			buffer[4096];
    ssize_t 		bytesRead;

	// Read from the stdoutFd of the CGI process
    bytesRead = read(cgi.stdoutFd, buffer, sizeof(buffer));

    if (bytesRead > 0)
    {
        // Process the CGI output (e.g., store it, send it to client, etc.)
        std::string output(buffer, bytesRead);
        // Example: send output to client
        buildResponse(output, cgi.clientFd);
    }
    else if (bytesRead == 0)
    {
        // No more data to read, handle completion if needed
        // Example: close CGI process, remove from _cgiProcesses, etc.
    }
    else
    {
        // Handle read error
        // Example: log error, handle exception, etc.
    }
	return;
	std::cout << pollIndex;
}

void	ServerManager::buildResponse(std::string content, int clientFd)
{
	std::string response = "";

	response = "HTTP/1.1 200 OK \r\n";
	response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += content;
	
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, GREEN);
}

t_CGIProcess& ServerManager::getCGIProcessForFd(int fd)
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

void	ServerManager::sendResponse(std::string response, int clientFd)
{
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, GREEN);
}

void	ServerManager::sendPendingResponse(int clientFd)
{
	sendResponse(clientResponses[clientFd], clientFd);
	clearFdForWriting(clientFd);
}

void	ServerManager::markFdForWriting(int clientFd)
{
    for (auto& pollFd : _pollFdsVector)
    {
        if (pollFd.fd == clientFd)
        {
            // Add POLLOUT flag to revents
            pollFd.events |= POLLOUT;
            return;
        }
    }
    // Handle case where clientFd is not found in _pollFdsVector
    throw std::runtime_error("Client fd not found in _pollFdsVector");
}

void	ServerManager::clearFdForWriting(int clientFd)
{
    for (auto& pollFd : _pollFdsVector)
    {
        if (pollFd.fd == clientFd)
        {
            // Remove POLLOUT flag from events
            pollFd.events &= ~POLLOUT;
            return;
        }
    }
    // Handle case where clientFd is not found in _pollFdsVector
    throw std::runtime_error("Client fd not found in _pollFdsVector");
}
