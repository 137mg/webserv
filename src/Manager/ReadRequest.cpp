/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ReadRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 12:56:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/24 15:09:13 by psadeghi         ###   ########.fr       */
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
	else {
		_clientBuffers[clientFd].append(buffer, bytes_read);
		ret = true;
		if (_clientBuffers[clientFd].size() > MB * 10)
			ret = false;
		if (getValue(_clientBuffers[clientFd], "Transfer-Encoding") == "chunked")
			return (handleChunkedRequest(_clientBuffers[clientFd], clientFd, ret));
		if (ret == false || isRequestComplete(_clientBuffers[clientFd]))
		{
			selectServer(_clientBuffers[clientFd], clientFd);
			markFdForWriting(clientFd);
			this->_clientBuffers.erase(clientFd);
		}
	}
	return (ret);
}


bool	Manager::handleChunkedRequest(std::string& buffer, int clientFd, bool ret)
{
	size_t	headerEnd;
	size_t	bodyEnd;
	size_t	lastEnd;
	size_t	newStart;

	headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return (ret);
	bodyEnd = buffer.find("0\r\n\r\n", headerEnd + 4);
	if (bodyEnd != std::string::npos)
	{
		buffer.erase(bodyEnd, 5);
		selectServer(buffer, clientFd);
		buffer.clear();
		return (ret);
	}
	lastEnd = buffer.find("\r\n", headerEnd + 4);
	newStart = buffer.find("\r\n", lastEnd + 2);
	buffer.erase(lastEnd, newStart - lastEnd + 2);
	return (ret);
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
