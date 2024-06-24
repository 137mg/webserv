/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:56:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/24 16:00:40 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

bool	Manager::readRequest(int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read;
	bool	ret;

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
	ret = true;
	if (isRequestTooLarge(_clientBuffers[clientFd]))
		ret = false;
	if (ret == false || isRequestComplete(_clientBuffers[clientFd]))
	{
		selectServer(_clientBuffers[clientFd], clientFd);
		markFdForWriting(clientFd);
		this->_clientBuffers.erase(clientFd);
	}
	return (ret);
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
	int		ret;

	headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (true);
	ret = true;
	if (buffer.size() - (headerEnd + 4) > MB * 10)
		ret = false;
	bodyEnd = buffer.find("0\r\n\r\n", headerEnd + 4);
	if (ret == false || bodyEnd != std::string::npos)
	{
		buffer.erase(bodyEnd, 5);
		selectServer(buffer, clientFd);
		markFdForWriting(clientFd);
		buffer.clear();
		return (ret);
	}
	lastEnd = buffer.find("\r\n", headerEnd + 4);
	newStart = buffer.find("\r\n", lastEnd + 2);
	buffer.erase(lastEnd, newStart - lastEnd + 2);
	return (true);
}
