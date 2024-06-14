/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:13:40 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/14 15:43:03 by mgoedkoo         ###   ########.fr       */
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
	if (this == &original)
		return (*this);
	_clientFd = original._clientFd;
	_request = original._request;
	_header = original._header;
	_location = original._location;
	port = original.port;
	clientMaxBodySize = original.clientMaxBodySize;
	host = original.host;
	defaultLocation = original.defaultLocation;
	serverNames = original.serverNames;
	locations = original.locations;
	errorPages = original.errorPages;
	errorMessages = original.errorMessages;
	return (*this);
}

void	Server::initDefaultLocation(void)
{
	defaultLocation.autoIndex = false;
	defaultLocation.match = "/";
	defaultLocation.root = ".";
	defaultLocation.allowedMethods.push_back("GET");
}

void	Server::initErrorPages(void)
{
	errorPages[400] = "html/error_pages/BadRequest.html"; // handled
	errorPages[401] = "html/error_pages/unauthorized.html";
	// status code 402 is not in use yet
	errorPages[403] = "html/error_pages/Forbidden.html"; // handled
	errorPages[404] = "html/error_pages/PageNotFound.html"; // handled
	errorPages[405] = "html/error_pages/MethodNotAllowed.html"; // handled
	errorPages[406] = "html/NotAcceptable.html";
	errorPages[407] = "html/ProxyAuthenticationRequired.html";
	errorPages[408] = "html/RequestTimeout.html";
	errorPages[409] = "html/Conflict.html";
	// status code 410 is for promotional purposes
	errorPages[411] = "html/error_pages/LengthRequired.html";
	// status code 412 seems out of scope
	errorPages[413] = "html/error_pages/PayloadTooLarge.html"; // handled
	errorPages[414] = "html/URITooLong.html";
	errorPages[415] = "html/error_pages/UnsupportedMediaType.html"; // handled
	// status codes 416-429 are unnecessary I think
	errorPages[431] = "html/error_pages/RequestHeaderFieldsTooLarge.html";
	// status code 451 deals with legal stuff, we are anarchists
	errorPages[500] = "html/error_pages/InternalServerError.html";
	errorPages[501] = "html/error_pages/NotImplemented.html";
	errorPages[502] = "html/error_pages/BadGateway.html";
	errorPages[503] = "html/error_pages/ServiceUnavailable.html";
	errorPages[504] = "html/error_pages/GatewayTimeout.html";
	errorPages[505] = "html/error_pages/HTTPVersionNotSupported.html";
	// status code 506 I don't even begin to understand
	errorPages[507] = "html/error_pages/InsufficientStorage.html";
	errorPages[508] = "html/error_pages/LoopDetected.html";
	// status code 510 seems out of scope too
	errorPages[511] = "html/error_pages/NetworkAuthenticationRequired.html";

	errorMessages[400] = "400 Bad Request";
	errorMessages[401] = "401 Unauthorized";
	errorMessages[403] = "403 Forbidden";
	errorMessages[404] = "404 Not Found";
	errorMessages[405] = "405 Method Not Allowed";
	errorMessages[406] = "406 Not Acceptable";
	errorMessages[407] = "407 Proxy Authentication Required";
	errorMessages[408] = "408 Request Timeout";
	errorMessages[409] = "409 Conflict";
	errorMessages[411] = "411 Length Required";
	errorMessages[413] = "413 Payload Too Large";
	errorMessages[414] = "414 URI Too Long";
	errorMessages[415] = "415 Unsupported Media Type";
	errorMessages[431] = "431 Request Header Fields Too Large";
	errorMessages[500] = "500 Internal Server Error";
	errorMessages[501] = "501 Not Implemented";
	errorMessages[502] = "502 Bad Gateway";
	errorMessages[503] = "503 Service Unavailable";
	errorMessages[504] = "504 Gateway Timeout";
	errorMessages[505] = "505 HTTP Version Not Supported";
	errorMessages[507] = "507 Insufficient Storage";
	errorMessages[508] = "508 Loop Detected";
	errorMessages[511] = "511 Network Authentication Required";
}

void	Server::checkServer(void)
{
	size_t	size;

	if (clientMaxBodySize > MB * 10)
		throw SizeTooLargeException();
	if (locations.empty())
		locations.push_back(defaultLocation);
	size = locations.size();
	for (size_t i = 0; i < size; i++)
	{
		checkLocation(locations[i]);
		for (size_t j = 0; j < size; j++)
		{
			if (i != j && locations[i].match == locations[j].match)
				throw LocationSyntaxException();
		}
	}
}

void	Server::checkLocation(t_location location)
{
	std::string	path;
	size_t		size;

	if (location.match[0] != '/')
		throw LocationSyntaxException();
	if (!location.index.empty())
	{
		path = location.root + location.match;
		if (path.back() != '/')
			path += "/";
		path += location.index;
		if (access(path.c_str(), R_OK) == -1)
			throw LocationPathException();
	}
	size = location.allowedMethods.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!(location.allowedMethods[i] == "GET"
			|| location.allowedMethods[i] == "POST"
			|| location.allowedMethods[i] == "DELETE"))
			throw LocationSyntaxException();
	}
	if (location.cgiExtents.size() != location.cgiPaths.size())
		throw LocationSyntaxException();
	size = location.cgiExtents.size();
	for (size_t i = 0; i < size; i++)
	{
		if (location.cgiExtents[i][0] != '.')
			throw LocationSyntaxException();
		if (access(location.cgiPaths[i].c_str(), X_OK) == -1)
			throw LocationPathException();
	}
}

const char*	Server::SizeTooLargeException::what(void) const throw()
{
	return ("Server: client max body size may not exceed 10MB");
}

const char*	Server::LocationSyntaxException::what(void) const throw()
{
	return ("Server: syntax error in location block");
}

const char*	Server::LocationPathException::what(void) const throw()
{
	return ("Server: path(s) in location block not accessible");
}
