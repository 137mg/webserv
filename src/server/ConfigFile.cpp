/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 17:42:09 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/15 19:51:36 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::configFile(const char* filename)
{
	std::ifstream	ifs(filename);

	if (ifs.fail())
		throw ConfigFileException();
	ifs.close();
}
