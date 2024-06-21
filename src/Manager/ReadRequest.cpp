/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   readRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 12:56:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/21 12:58:43 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

// Handle the connection with a client, reading incoming data and processing requests
bool	Manager::readRequest(int clientFd)
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
		std::cerr << RED << BOLD << "Read error on socket " << clientFd << " " << std::strerror(errno) << RESET << std::endl;
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

bool Manager::isRequestComplete(const std::string &request_buffer)
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
size_t Manager::getRequestSize(std::string request_buffer)
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
