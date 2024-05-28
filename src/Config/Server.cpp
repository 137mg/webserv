/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:13:40 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/28 15:47:22 by mgoedkoo         ###   ########.fr       */
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
	errorPages[400] = "";
	errorPages[401] = "";
	// status code 402 is not in use yet
	errorPages[403] = "";
	errorPages[404] = "html/PageNotFound.html";
	errorPages[405] = "";
	errorPages[406] = "";
	errorPages[407] = "";
	errorPages[408] = "";
	errorPages[409] = "";
	// status code 410 is for promotional purposes
	errorPages[411] = "";
	// status code 412 seems out of scope
	errorPages[413] = "";
	errorPages[414] = "";
	errorPages[415] = "";
	// status codes 416-429 are unnecessary I think
	errorPages[431] = "";
	// status code 451 deals with legal stuff, we are anarchists
	errorPages[500] = "";
	errorPages[501] = "";
	errorPages[502] = "";
	errorPages[503] = "";
	errorPages[504] = "";
	errorPages[505] = "";
	// status code 506 I don't even begin to understand
	errorPages[507] = "";
	errorPages[508] = "";
	// status code 510 seems out of scope too
	errorPages[511] = "";
}

bool	Server::checkServer(void)
{
	std::deque<t_location>::iterator	it1;
	std::deque<t_location>::iterator	it2;
	std::deque<t_location>::iterator	ite;

	if (port == 0)
		return (false);
	if (locations.empty())
		locations.push_back(defaultLocation);
	ite = locations.end();
	for (it1 = locations.begin(); it1 != ite; it1++)
	{
		if (!checkLocation(*it1))
			return (false);
		for (it2 = locations.begin(); it2 != ite; it2++)
		{
			if (it1 != it2 && it1->match == it2->match)
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
