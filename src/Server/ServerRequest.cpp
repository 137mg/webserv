/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/06 12:40:04 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// need to decide what error to throw when needed variable is not found!
static std::string	getValue(std::string request, std::string key)
{
	std::string	value;
	size_t		startLine;
	size_t		startValue;
	size_t		endLine;
	
	startLine = request.find(key + ": ");
	if (startLine == std::string::npos)
		return ("");
	endLine = request.find("\r\n", startLine);
	if (endLine == std::string::npos)
		return ("");
	startValue = startLine + key.length() + 2;
	value = request.substr(startValue, endLine - startValue);
	return (value);
}

t_header	Server::parseRequest(void)
{
	std::istringstream	iss(_request);
	t_header			header;

	std::getline(iss, header.method, ' ');
	std::getline(iss, header.file, ' ');
	std::getline(iss, header.protocol, '\r');
	if (header.method == "POST")
	{
		header.contentLength = getValue(_request, "Content-Length");
		header.contentType = getValue(_request, "Content-Type");
	}
	return (header);
}

t_location Server::selectLocation(void)
{
	t_location	location;
	size_t		size;
	size_t		length;

	size = locations.size();
	length = 0;
	for (size_t i = 0; i < size; i++)
	{
		if (_header.file.find(locations[i].match) != std::string::npos)
		{
			if (locations[i].match.length() > length)
			{
				location = locations[i];
				length = locations[i].match.length();
			}
		}
	}
	return (location);
}

void	Server::handleRequest(std::string request, int clientFd)
{
	_clientFd = clientFd;
	_request = request;
	_header = parseRequest();
	_location = selectLocation();
	clientMessage(_request, clientFd);
	if (_request.size() > 1048576)
	{
		send413Response(clientFd);
		return;
	}
	if (_header.method == "GET")
		getMethod();
	else if (_header.method == "DELETE")
		deleteMethod();
	else if (_header.method == "POST")
		postMethod();
}
