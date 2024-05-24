/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/24 19:23:07 by mirjam           ###   ########.fr       */
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

static bool	isWhitespace(char c)
{
	if (std::isspace(c))
		return (true);
	return (false);
}

void	Config::removeWhitespace(void)
{
	std::string::iterator	it;
	std::string::iterator	ite;

	it = _line.begin();
	ite = _line.end();
	ite = remove_if(it, ite, isWhitespace);
	_line.resize(ite - it);
}

void	Config::parseFile(void)
{
	for (getline(_ifs, _line); !_ifs.eof(); getline(_ifs, _line))
	{
		removeWhitespace();
		if (!_line.empty())
			break;
	}
	if (_line == "[[server]]")
		addServer();
	else
		throw ConfigFileException();
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

	for (getline(_ifs, _line);
		!(_ifs.eof() && _line.empty()); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		updateServer(server);
	}
	if (!server.checkServer())
		throw ConfigFileException();
	servers.push_front(server);
}

void	Config::addErrorPages(Server& server)
{
	for (getline(_ifs, _line);
		!(_ifs.eof() && _line.empty()); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		updateErrorPages(server);
	}
}

void	Config::addLocation(Server& server)
{
	t_location	location;

	location = server.defaultLocation;
	for (getline(_ifs, _line);
		!(_ifs.eof() && _line.empty()); getline(_ifs, _line))
	{
		removeWhitespace();
		if (_line.empty())
			continue;
		if (newTable(server))
			break;
		updateLocation(location);
	}
	server.locations.push_front(location);
}

void	Config::updateServer(Server& server)
{
	// parsing
	updateLocation(server.defaultLocation);
}

void	Config::updateErrorPages(Server& server)
{
	size_t		i;
	int			tmp;
	uint16_t	code;
	std::string	key;
	std::string	value;

	i = _line.find('=');
	if (i != 3)
		throw ConfigFileException();
	key = _line.substr(0, i);
	value = _line.substr(i + 1);
	// check access value?
	tmp = stoi(key);
	if (tmp < 0 || tmp > 65535)
		throw ConfigFileException();
	code = static_cast<uint16_t>(tmp);
	if (server.errorPages.count(code) == 0)
		throw ConfigFileException();
	server.errorPages[code] = value;
}

void	Config::updateLocation(t_location& location)
{
	// parsing
	(void)location;
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
