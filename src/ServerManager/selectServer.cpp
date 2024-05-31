/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   selectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 16:39:49 by mgoedkoo         ###   ########.fr       */
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
