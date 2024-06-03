/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   selectServer.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/03 15:54:09 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// placeholder
void	ServerManager::selectServer(std::string buffer, int clientFd)
{
	uint16_t			port;
	std::vector<Server>	serverList;
	Server				server;

	port = _ports[0];
	serverList = _serverMap[port];
	server = serverList[0];
	server.parseRequest(buffer, clientFd);
}
