/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 17:42:09 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/14 14:16:13 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Config.hpp"

void	ServerManager::configFile(const char* filename)
{
	size_t		size;
	uint16_t	port;

	Config	config(filename);
	config.parseFile();
	//config.printServers();
	size = config.servers.size();
	for (size_t i = 0; i < size; i++)
	{
		port = config.servers[i].port;
		if (_serverMap.count(port) == 0)
			_ports.push_back(port);
		_serverMap[port].push_back(config.servers[i]);
	}
}
