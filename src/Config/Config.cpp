/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/21 17:56:32 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(const char* filename)
{
	std::ifstream	ifs(filename);

	if (!ifs.good())
		throw ConfigFileException();
	std::getline(ifs, _fileContent, '\0');
	if (ifs.bad())
		throw ConfigFileException();
	ifs.close();
	_defaultLocation.root = "html/";
	_defaultLocation.index = "home.html";
	_defaultLocation.autoindex = false;
}

Config::~Config(void)
{
}

size_t	Config::findOpenBracket(size_t begin)
{
	size_t	openBracket;

	openBracket = _fileContent.find("{", begin);
	if (openBracket == std::string::npos)
		throw ConfigFileException();
	return (openBracket);
}

size_t	Config::findCloseBracket(size_t begin)
{
	size_t	closeBracket;

	closeBracket = _fileContent.find("}", begin);
	if (closeBracket == std::string::npos)
		throw ConfigFileException();
	return (closeBracket);
}

void	Config::parseFile(void)
{
	size_t	serverBlock;
	size_t	openBracket;
	size_t	closeBracket;

	serverBlock = _fileContent.find("server");
	while (serverBlock != std::string::npos)
	{
		openBracket = findOpenBracket(serverBlock + 6);
		closeBracket = findCloseBracket(openBracket + 1);
		addServer(openBracket, closeBracket);
		serverBlock = _fileContent.find("server", closeBracket);
	}
	// if (vServers.empty())
	// 	throw ConfigFileException();
}

void	Config::addServer(size_t begin, size_t end)
{
	(void)begin;
	(void)end;
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
