/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 16:30:52 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void)
{
}

Config::Config(const char* filename) : _ifs(filename)
{
	if (!_ifs.good())
		throw ConfigFileException();
}

Config::Config(const Config& original)
{
	(void)original;
}

Config&	Config::operator=(const Config& original)
{
	(void)original;
	return (*this);
}

Config::~Config(void)
{
	_ifs.close();
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}