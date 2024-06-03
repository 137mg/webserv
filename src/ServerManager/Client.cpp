/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 17:00:22 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/03 14:12:18 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

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
		return false;
	}
	// still need to decide between exception and error page or the like!
	else if (bytes_read < 0)
	{
		std::cerr << RED << BOLD << "Read error " << std::strerror(errno) << RESET << std::endl;
		return false;
	}
	else
	{
		// Append the read data to the client's buffer
		std::string& clientBuffer = this->_clientBuffers[clientFd];
		clientBuffer.append(buffer, bytes_read);
		
		if (isRequestComplete(clientBuffer))
		{
			selectServer(clientBuffer, clientFd);
			this->_clientBuffers.erase(clientFd);
		}
	}
	return true;
}

bool ServerManager::isRequestComplete(const std::string &request_buffer)
{
	size_t	headerEnd;
	size_t	totalExpectedSize;
	size_t	contentLengthPos;

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
