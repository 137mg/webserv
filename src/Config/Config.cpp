/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/23 15:09:22 by mgoedkoo         ###   ########.fr       */
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

void	Config::removeWhitespace(std::string& line)
{
	std::string::iterator	it;
	std::string::iterator	ite;

	it = line.begin();
	ite = line.end();
	ite = remove_if(it, ite, isWhitespace);
	line.resize(ite - it);
}

void	Config::parseFile(void)
{
	getline(_ifs, _line);
	while (!_ifs.eof())
	{
		removeWhitespace(_line);
		if (_line == "[[server]]")
		{
			addServer();
			break;
		}
		else if (!_line.empty())
			throw ConfigFileException();
		getline(_ifs, _line);
	}
	removeWhitespace(_line);
	if (!_line.empty() || _servers.empty())
		throw ConfigFileException();
}

void	Config::addServer(void)
{
	Server		server;

	getline(_ifs, _line);
	while (!_ifs.eof())
	{
		removeWhitespace(_line);
		if (_line == "[[server]]")
		{
			addServer();
			break;
		}
		getline(_ifs, _line);
	}
	// server.checkServer();
}

void	Config::addLocation(void)
{
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
