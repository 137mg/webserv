/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/28 16:20:23 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(const char* filename) : _ifs(filename)
{
	std::string	nameCheck;

	if (!_ifs.good())
		throw ConfigFileException();
	nameCheck = filename;
	if (nameCheck.rfind(".toml") != nameCheck.size() - 5)
		throw ConfigFileException();
}

Config::~Config(void)
{
	_ifs.close();
}

void	Config::parseFile(void)
{
	size_t	size;

	for (getline(_ifs, _line); !_ifs.eof(); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (_line == "[[server]]")
		{
			addServer();
			break;
		}
		else
			throw ConfigFileException();
	}
	removeWhitespace();
	if (!_line.empty() || servers.empty())
		throw ConfigFileException();
	size = servers.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!servers[i].checkServer())
			throw ConfigFileException();
	}
}

bool	Config::newTable(Server& server)
{
	if (_line[0] != '[')
		return (false);
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
	throw ConfigFileException();
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

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
