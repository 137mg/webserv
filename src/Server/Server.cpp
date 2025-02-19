/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:13:40 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/07/04 12:49:04 by mgoedkoo         ###   ########.fr       */
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
	_lastPath = original._lastPath;
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
	Manager = original.Manager;
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
	errorPages[400] = "html/error_pages/BadRequest.html";
	errorPages[403] = "html/error_pages/Forbidden.html";
	errorPages[404] = "html/error_pages/PageNotFound.html";
	errorPages[405] = "html/error_pages/MethodNotAllowed.html";
	errorPages[411] = "html/error_pages/LengthRequired.html";
	errorPages[413] = "html/error_pages/PayloadTooLarge.html";
	errorPages[415] = "html/error_pages/UnsupportedMediaType.html";
	errorPages[500] = "html/error_pages/InternalServerError.html";
	errorPages[505] = "html/error_pages/HTTPVersionNotSupported.html";

	errorMessages[400] = "400 Bad Request";
	errorMessages[403] = "403 Forbidden";
	errorMessages[404] = "404 Not Found";
	errorMessages[405] = "405 Method Not Allowed";
	errorMessages[408] = "408 Request Timeout";
	errorMessages[411] = "411 Length Required";
	errorMessages[413] = "413 Payload Too Large";
	errorMessages[415] = "415 Unsupported Media Type";
	errorMessages[500] = "500 Internal Server Error";
	errorMessages[505] = "505 HTTP Version Not Supported";
}

void	Server::checkServer(void)
{
	size_t	size;

	if (clientMaxBodySize > MB * 10)
		throw StandardValuesException();
	if (host != "127.0.0.1")
		throw StandardValuesException();
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
			throw StandardValuesException();
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

const char*	Server::StandardValuesException::what(void) const throw()
{
	return ("Server: host must be 127.0.0.1, client_max_body_size may not exceed 10MB,\n\
and ony GET, POST and DELETE methods are allowed");
}

const char*	Server::LocationSyntaxException::what(void) const throw()
{
	return ("Server: syntax error in location block");
}

const char*	Server::LocationPathException::what(void) const throw()
{
	return ("Server: path(s) in location block not accessible");
}
