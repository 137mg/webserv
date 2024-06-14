/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/14 14:55:51 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(const char* filename) : _ifs(filename)
{
	std::string	nameCheck;

	if (!_ifs.good())
		throw InvalidFileException();
	nameCheck = filename;
	if (nameCheck.rfind(".toml") != nameCheck.size() - 5)
		throw InvalidFileException();
}

Config::~Config(void)
{
	_ifs.close();
}

void	Config::parseFile(void)
{
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
			throw SyntaxErrorException();
	}
	removeWhitespace();
	if (!_line.empty() || servers.empty())
		throw SyntaxErrorException();
}

const char*	Config::InvalidFileException::what(void) const throw()
{
	return ("Configuration file: file in wrong format or not accessible");
}

const char*	Config::SyntaxErrorException::what(void) const throw()
{
	return ("Configuration file: syntax error");
}

const char*	Config::OutOfRangeException::what(void) const throw()
{
	return ("Configuration file: numbers must be between 1 and 65535");
}

const char*	Config::ErrorPageException::what(void) const throw()
{
	return ("Configuration file: invalid code or path for error page");
}
