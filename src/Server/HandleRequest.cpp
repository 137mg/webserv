/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   HandleRequest.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 17:50:46 by juvan-to      ########   odam.nl         */
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

static bool	checkHeader(t_header header)
{
	if (header.method.empty() || header.file.empty() || header.protocol.empty())
		return (false);
	if (header.host.empty() || header.port == 0)
		return (false);
	if (header.method == "POST" && (header.contentLength.empty() || header.contentType.empty()))
		return (false);
	return (true);
}

int	Server::checkRequest(void)
{
	if (_header.method == "POST" && _header.contentLength.empty())
		return (411);
	if (!checkHeader(_header))
		return (400);
	if (_header.protocol != "HTTP/1.1")
		return (505);
	if (!_header.contentLength.empty() && stoul(_header.contentLength) > clientMaxBodySize)
		return (413);
	if (_location.match.empty())
		return (404);
	if (!_location.redirect.empty())
		return (301);
	return (0);
}

void	Server::handleRequest(t_header header, std::string request, int clientFd)
{
	int		code;
	size_t	size;

	clientMessage(request, clientFd);
	_clientFd = clientFd;
	_request = request;
	_header = header;
	_location = selectLocation();
	code = checkRequest();
	if (code != 0)
	{
		buildErrorResponse(code);
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
	buildErrorResponse(405);
}
