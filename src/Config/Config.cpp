/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:59:10 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 16:14:58 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(const char* filename)
{
	std::ifstream	ifs(filename);

	if (ifs.fail())
		throw ConfigFileException();
	ifs.close();
}

Config::~Config(void)
{
}

const char*	Config::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}
