/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigPrint.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mirjam <mirjam@student.42.fr>                +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/28 15:39:49 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/12 14:21:54 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

static void	printLocation(t_location& location)
{
	size_t	size;

	std::cout << "autoindex: " << location.autoIndex << std::endl;
	std::cout << "match: " << location.match << std::endl;
	std::cout << "root: " << location.root << std::endl;
	std::cout << "index: " << location.index << std::endl;
	std::cout << "redirect: " << location.redirect << std::endl;
	std::cout << "allowed methods:" << std::endl;
	size = location.allowedMethods.size();
	for (size_t i = 0; i < size; i++)
		std::cout << location.allowedMethods[i] << std::endl;
	std::cout << "cgi extents:" << std::endl;
	size = location.cgiExtents.size();
	for (size_t i = 0; i < size; i++)
		std::cout << location.cgiExtents[i] << std::endl;
	std::cout << "cgi methods:" << std::endl;
	size = location.cgiPaths.size();
	for (size_t i = 0; i < size; i++)
		std::cout << location.cgiPaths[i] << std::endl;
}

static void	printServer(Server& server)
{
	size_t	size;

	std::cout << "port: " << server.port << std::endl;
	std::cout << "client max body size: " << server.clientMaxBodySize << std::endl;
	std::cout << "host: " << server.host << std::endl;
	std::cout << "server names:" << std::endl;
	size = server.serverNames.size();
	for (size_t i = 0; i < size; i++)
		std::cout << server.serverNames[i] << std::endl;
	std::cout << std::endl;
	size = server.locations.size();
	for (size_t i = 0; i < size; i++)
	{
		std::cout << "---- LOCATION ----" << std::endl;
		printLocation(server.locations[i]);
		std::cout << std::endl;
	}
}

void	Config::printServers(void)
{
	size_t	size;

	size = servers.size();
	for (size_t i = 0; i < size; i++)
	{
		std::cout << "-------- SERVER --------" << std::endl;
		printServer(servers[i]);
	}
}
