/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ReadRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 12:56:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/24 18:44:05 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

bool	Manager::readRequest(int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read;

	memset(buffer, '\0', MESSAGE_BUFFER);
	bytes_read = read(clientFd, buffer, MESSAGE_BUFFER - 1);
	if (bytes_read == 0 && _clientActivityMap[_clientFd] != 0)
	{
		printTimestamp();
		std::cout << GREEN << "Client socket " << RESET << clientFd << RED << " closed " << RESET << "the connection." << std::endl;
		this->_clientBuffers.erase(clientFd);
		return (false);
	}
	if (bytes_read < 0)
	{
		std::cerr << RED << BOLD << "Read error " << std::strerror(errno) << RESET << std::endl;
		return (false);
	}
	_clientBuffers[clientFd].append(buffer, bytes_read);
	if (getValue(_clientBuffers[clientFd], "Transfer-Encoding") == "chunked")
		return (handleChunkedRequest(_clientBuffers[clientFd], clientFd));
	if (isRequestTooLarge(_clientBuffers[clientFd]) || isRequestComplete(_clientBuffers[clientFd]))
	{
		_clientStatus[clientFd] = WRITING;
		selectServer(_clientBuffers[clientFd], clientFd);
		markFdForWriting(clientFd);
		this->_clientBuffers.erase(clientFd);
	}
	return (true);
}

bool	Manager::isRequestTooLarge(std::string buffer)
{
	std::string	contentLength;
	size_t		number;

	contentLength = getValue(buffer, "Content-Length");
	if (!contentLength.empty())
	{
		number = std::stoi(contentLength);
		if (number > MB * 10)
			return (true);
	}
	return (false);
}

bool	Manager::isRequestComplete(std::string buffer)
{
	std::string	contentLength;
	size_t		number;
	size_t		headerEnd;
	size_t		totalExpectedSize;

	headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (false);
	contentLength = getValue(buffer, "Content-Length");
	if (!contentLength.empty())
	{
		number = std::stoi(contentLength);
		totalExpectedSize = headerEnd + 4 + number;
		if (buffer.size() >= totalExpectedSize)
			return (true);
		return (false);
	}
	return (true);
}

bool	Manager::handleChunkedRequest(std::string& buffer, int clientFd)
{
	size_t	headerEnd;
	size_t	bodyEnd;
	size_t	lastEnd;
	size_t	newStart;

	headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (true);
	bodyEnd = buffer.find("0\r\n\r\n", headerEnd + 4);
	if (bodyEnd != std::string::npos)
	{
		buffer.erase(bodyEnd, 5);
		selectServer(buffer, clientFd);
		markFdForWriting(clientFd);
		buffer.clear();
		return (true);
	}
	lastEnd = buffer.find("\r\n", headerEnd + 4);
	newStart = buffer.find("\r\n", lastEnd + 2);
	buffer.erase(lastEnd, newStart - lastEnd + 2);
	return (true);
}
