/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 17:42:09 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/21 15:57:41 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Config.hpp"

void	Manager::configFile(const char* filename)
{
	size_t		size;
	uint16_t	port;

	Config	config(filename);
	config.parseFile();
	//config.printServers();
	size = config.servers.size();
	for (size_t i = 0; i < size; i++)
	{
		config.servers[i].Manager = this;
		port = config.servers[i].port;
		if (_serverMap.count(port) == 0)
			_ports.push_back(port);
		_serverMap[port].push_back(config.servers[i]);
	}
}
