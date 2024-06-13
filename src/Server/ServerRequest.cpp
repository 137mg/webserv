/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/13 15:20:39 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

void	Server::handleRequest(t_header header, std::string request, int clientFd)
{
	size_t	size;

	clientMessage(request, clientFd);
	_clientFd = clientFd;
	_request = request;
	_header = header;
	_location = selectLocation();
	if (_location.match.empty())
	{
		sendErrorResponse(404);
		return;
	}
	if (!_header.contentLength.empty() && stoul(_header.contentLength) > clientMaxBodySize)
	{
		sendErrorResponse(413);
		return;
	}
	size = _location.allowedMethods.size();
	for (size_t i = 0; i < size; i++)
	{
		if (_header.method == _location.allowedMethods[i])
		{
			if (_header.method == "GET")
				getMethod();
			else if (_header.method == "DELETE")
				deleteMethod();
			else if (_header.method == "POST")
				postMethod();
			return;
		}
	}
	sendErrorResponse(405);
}
