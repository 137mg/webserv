/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManagerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:46:03 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 15:46:06 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

ServerManager::ServerManager(void)
{	
	this->_port = PORT;
	this->_ServerManagerName = "Webserv";
	this->_root = "";
	this->_index = "";
	this->_listenFd = -1;
	this->_clientMaxBodySize = 1;
	this->_autoIndex = false;
}

ServerManager::~ServerManager(void)
{
	if (this->_listenFd != -1)
		close(this->_listenFd);
}

ServerManager::ServerManager(const ServerManager &other)
{
	*this = other;
}

ServerManager & ServerManager::operator=(const ServerManager &other)
{
	if (this != &other)
	{
		this->_port = other._port;
		this->_listenFd = other._listenFd;
		this->_clientFd = other._clientFd;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_ServerManagerName = other._ServerManagerName;
		this->_root = other._root;
		this->_autoIndex = other._autoIndex;
		this->_ServerManagerAddress = other._ServerManagerAddress;
	}
	return *this;
}

void	ServerManager::config(void)
{
	this->_ServerManagerAddress.sin_family = AF_INET;
	this->_ServerManagerAddress.sin_addr.s_addr = INADDR_ANY;
	this->_ServerManagerAddress.sin_port = htons(PORT);

	this->createSocket();
	this->bindSocket();
	printTimestamp();
	std::cout << PURPLE << UNDER << this->_ServerManagerName << RESET << " up and running. Listening on port: "
		<< UNDER << this->_port << RESET << std::endl;
	std::cout << "-----------------------------------------------------------------------" << std::endl;
}

// Create a socket for the ServerManager to listen for incoming connections
void	ServerManager::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_ServerManagerAddress.sin_family , SOCK_STREAM, 0);
	if (this->_listenFd == -1)
		throw ServerSocketException();
	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
    if (setsockopt(this->_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw ServerSocketException();
}

// Bind the socket to a specific address and port and listen for incoming connections
void	ServerManager::bindSocket(void)
{	
	if (bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_ServerManagerAddress),
		sizeof(this->_ServerManagerAddress)) != 0)
		throw ServerSocketException();

	if (listen(this->_listenFd, BACKLOG) != 0)
		throw ServerSocketException();
}

// Keep accepting connections while the ServerManager is running
void	ServerManager::run(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;
	
	addr_size = sizeof(client_addr);
	this->_clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (this->_clientFd == -1)
		throw ClientSocketException();
	fcntl(this->_clientFd, F_SETFL, O_NONBLOCK);
	while (1)
	{
		if(!this->handleClientConnection())
			break;
	}
	printTimestamp();
	std::cout << RED << "Closing " << RESET << "client socket " << RESET << this->_clientFd << std::endl;
	close(this->_clientFd);
	return;
}

const char*	ServerManager::ServerSocketException::what(void) const throw()
{
	return ("Server socket: ");
}

const char*	ServerManager::ClientSocketException::what(void) const throw()
{
	return ("Client socket: ");
}
