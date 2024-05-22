/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/22 17:22:34 by mgoedkoo         ###   ########.fr       */
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
}

Config::~Config(void)
{
}

void	Config::parseFile(void)
{
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
