/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/16 17:13:40 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/05 14:23:12 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{
	port = 8080;
	clientMaxBodySize = MB;
	host = "127.0.0.1";
	initDefaultLocation();
	initErrorPages();
}

Server::Server(const Server& original)
{
	*this = original;
}

Server::~Server(void)
{
}

Server&	Server::operator=(const Server& original)
{
	port = original.port;
	clientMaxBodySize = original.clientMaxBodySize;
	host = original.host;
	defaultLocation = original.defaultLocation;
	serverNames = original.serverNames;
	locations = original.locations;
	errorPages = original.errorPages;
	return (*this);
}

void	Server::initDefaultLocation(void)
{
	defaultLocation.autoIndex = false;
	defaultLocation.match = "/";
	defaultLocation.root = "html/";
	defaultLocation.index = "home.html";
	defaultLocation.allowedMethods.push_back("GET");
}

void	Server::initErrorPages(void)
{
	errorPages[400] = "html/BadRequest.html";
	errorPages[401] = "html/unauthorized.html";
	// status code 402 is not in use yet
	errorPages[403] = "html/error_pages/Forbidden.html";
	errorPages[404] = "html/error_pages/PageNotFound.html";
	errorPages[405] = "html/MethodNotAllowed.html";
	errorPages[406] = "html/NotAcceptable.html";
	errorPages[407] = "html/ProxyAuthenticationRequired.html";
	errorPages[408] = "html/RequestTimeout.html";
	errorPages[409] = "html/Conflict.html";
	// status code 410 is for promotional purposes
	errorPages[411] = "html/LengthRequired.html";
	// status code 412 seems out of scope
	errorPages[413] = "html/error_pages/PayloadTooLarge.html";
	errorPages[414] = "html/URITooLong.html";
	errorPages[415] = "html/UnsupportedMediaType.html";
	// status codes 416-429 are unnecessary I think
	errorPages[431] = "html/RequestHeaderFieldsTooLarge.html";
	// status code 451 deals with legal stuff, we are anarchists
	errorPages[500] = "html/InternalServerError.html";
	errorPages[501] = "html/NotImplemented.html";
	errorPages[502] = "html/BadGateway.html";
	errorPages[503] = "html/ServiceUnavailable.html";
	errorPages[504] = "html/GatewayTimeout.html";
	errorPages[505] = "html/HTTPVersionNotSupported.html";
	// status code 506 I don't even begin to understand
	errorPages[507] = "html/InsufficientStorage.html";
	errorPages[508] = "html/LoopDetected.html";
	// status code 510 seems out of scope too
	errorPages[511] = "html/NetworkAuthenticationRequired.html";
}

bool	Server::checkServer(void)
{
	size_t	size;

	if (port == 0)
		return (false);
	if (clientMaxBodySize == 0 || clientMaxBodySize > MB * 10)
		return (false);
	if (locations.empty())
		locations.push_back(defaultLocation);
	size = locations.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!checkLocation(locations[i]))
			return (false);
		for (size_t j = 0; j < size; j++)
		{
			if (i != j && locations[i].match == locations[j].match)
				return (false);
		}
	}
	return (true);
}

bool	Server::checkLocation(t_location location)
{
	std::string	path;
	size_t		size;

	if (location.match[0] != '/')
		return (false);
	path = location.root + location.index;
	if (access(path.c_str(), R_OK) == -1)
		return (false);
	size = location.allowedMethods.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!(location.allowedMethods[i] == "GET"
			|| location.allowedMethods[i] == "POST"
			|| location.allowedMethods[i] == "DELETE"))
			return (false);
	}
	if (location.cgiExtents.size() != location.cgiPaths.size())
		return (false);
	size = location.cgiExtents.size();
	for (size_t i = 0; i < size; i++)
	{
		if (location.cgiExtents[i][0] != '.')
			return (false);
		if (access(location.cgiPaths[i].c_str(), X_OK) == -1)
			return (false);
	}
	return (true);
}
