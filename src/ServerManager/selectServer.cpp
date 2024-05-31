/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   selectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 16:06:41 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// placeholder
void	ServerManager::selectServer(std::string buffer, int clientFd)
{
	Server	server;

	server = _serverMap[_ports[0]][0];
	server.parseRequest(buffer, clientFd);
}
