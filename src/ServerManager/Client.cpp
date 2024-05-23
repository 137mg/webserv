/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:00:22 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/23 15:50:59 by psadeghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// Handle the connection with a client, reading incoming data and processing requests
// bool	ServerManager::handleClientConnection(int clientFd)
// {
//     std::string request_buffer;
// 	int			bytes_read;

//     while (1)
// 	{
// 		bytes_read = readFromSocket(request_buffer, clientFd);
//         if (bytes_read < 0)
//         	return false;
// 		else if (bytes_read == 0)
// 			continue;
// 		else
// 		{
// 			request_buffer.append(this->_buffer);
// 			if (isRequestComplete(request_buffer))
// 			{
// 				handleRequest(request_buffer, clientFd);
// 				break;
// 			}
// 		}
// 	}
// 	return true;
// }

bool ServerManager::handleClientConnection(int clientFd)
{
	int bytes_read = recv(clientFd, this->_buffer, MESSAGE_BUFFER, 0);
	if (bytes_read > 0) {
		this->clientBuffers[clientFd].append(this->_buffer, bytes_read);
		if (isRequestComplete(this->clientBuffers[clientFd])) {
			handleRequest(this->clientBuffers[clientFd], clientFd);
			this->clientBuffers[clientFd].clear(); // Clear the buffer for the next request
		}
		return true;
	} else if (bytes_read == 0) {
		// Client closed connection
		return false;
	}
	// For non-blocking sockets, a return value of -1 with no error check means we continue polling
	return true;
}

// Read data from the client socket into a buffer
int ServerManager::readFromSocket(std::string &outbuffer, int clientFd)
{
	char	buffer[MESSAGE_BUFFER] = {0};
	int		bytes_read = recv(clientFd, buffer, MESSAGE_BUFFER, 0);
	
	if (bytes_read > 0)
		outbuffer.append(buffer, bytes_read);
	if (bytes_read == -1)
	{
		std::cerr << RED << BOLD << "recv error " << std::strerror(errno) << RESET << std::endl;
		return 0;
	}
	if (bytes_read <= 0)
	{
		close(clientFd);
		return -1;
	}
	// else if (bytes_read == 0)
	// {
	// 	printTimestamp();
	// 	std::cout << GREEN << "Client socket " << RESET << clientFd << RED << " closed " << RESET << "the connection." << std::endl;
	// 	return -1;
	// }
	// else
	// {
	// 	if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 		return 0;
	// 	else
	// 	{
	// 		std::cerr << RED << BOLD << "recv error " << std::strerror(errno) << RESET << std::endl;
	// 		return -1;
	// 	}
	// }
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
