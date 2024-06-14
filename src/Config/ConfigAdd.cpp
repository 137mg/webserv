/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigAdd.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 14:13:56 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/14 14:56:11 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

bool	Config::newTable(Server& server)
{
	if (_line == "[[server]]")
	{
		addServer();
		return (true);
	}
	if (_line == "[server.error_pages]")
	{
		addErrorPages(server);
		return (true);
	}
	if (_line == "[[server.location]]")
	{
		addLocation(server);
		return (true);
	}
	return (false);
}

void	Config::addServer(void)
{
	Server	server;

	for (getline(_ifs, _line); !_ifs.eof(); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		parseLine();
		updateServer(server);
	}
	server.checkServer();
	servers.push_front(server);
}

void	Config::addErrorPages(Server& server)
{
	for (getline(_ifs, _line); !_ifs.eof(); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		parseLine();
		updateErrorPages(server);
	}
}

void	Config::addLocation(Server& server)
{
	t_location	location;

	location = server.defaultLocation;
	for (getline(_ifs, _line); !_ifs.eof(); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		parseLine();
		updateLocation(location);
	}
	server.locations.push_front(location);
}
