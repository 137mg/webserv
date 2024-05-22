/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 17:00:22 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/22 14:22:04 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// Handle the connection with a client, reading incoming data and processing requests
bool	ServerManager::handleClientConnection(int clientFd)
{
    std::string request_buffer;
	int			bytes_read;

	bytes_read = readFromSocket(request_buffer, clientFd);
	if (bytes_read < 0)
		return false;
	else if (bytes_read == 0)
		return true;
	else
	{
		request_buffer.append(this->_buffer);
		if (isRequestComplete(request_buffer))
		{
			handleRequest(request_buffer, clientFd);
			return true;
		}
	}
    // while (1)
	// {
	// 	bytes_read = readFromSocket(request_buffer, clientFd);
    //     if (bytes_read < 0)
    //     	return false;
	// 	else if (bytes_read == 0)
	// 		continue;
	// 	else
	// 	{
	// 		request_buffer.append(this->_buffer);
	// 		if (isRequestComplete(request_buffer))
	// 		{
	// 			handleRequest(request_buffer, clientFd);
	// 			break;
	// 		}
	// 	}
	// }
	return true;
}

// Read data from the client socket into a buffer
int ServerManager::readFromSocket(std::string &outbuffer, int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read;
	
	bytes_read = recv(clientFd, buffer, MESSAGE_BUFFER - 1, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read == -1)
			std::cerr << RED << BOLD << "recv: reading error " << std::strerror(errno) << RESET << std::endl;
		return -1;
	}
	outbuffer.append(buffer, bytes_read);
	return bytes_read;
}

bool ServerManager::isRequestComplete(const std::string &request_buffer)
{
	size_t	headerEnd;
	size_t totalExpectedSize;
	size_t contentLengthPos;

	headerEnd = request_buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;
	totalExpectedSize = getRequestSize(request_buffer);
	contentLengthPos = request_buffer.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		if (request_buffer.size() >= totalExpectedSize)
			return true;
	} 
	else
		return true;
	return false;
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
		return totalExpectedSize;
	} 
	return request_buffer.size();
}
