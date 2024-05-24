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
	this->_ServerName = "Webserv";
	this->_root = "";
	this->_index = "";
	this->_listenFd = -1;
	this->_clientMaxBodySize = 1;
	this->_autoIndex = false;
//	this->_pollFds = new struct pollfd[5];
	this->_pollCount = 5;
	this->_pollSize = 0;
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
		this->_ServerName = other._ServerName;
		this->_root = other._root;
		this->_autoIndex = other._autoIndex;
		this->_ServerAddress = other._ServerAddress;
		this->_pollFds = other._pollFds;
		this->_pollSize = other._pollSize;
		this->_pollCount = other._pollCount;
	}
	return *this;
}

void	ServerManager::config(void)
{
	this->_ServerAddress.sin_family = AF_INET;
	this->_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	this->_ServerAddress.sin_port = htons(PORT);

	this->createSocket();
	this->bindSocket();
	printTimestamp();
	std::cout << PURPLE << UNDER << this->_ServerName << RESET << " up and running. Listening on port: "
		<< UNDER << this->_port << RESET << std::endl;
	std::cout << "-----------------------------------------------------------------------" << std::endl;
}

// Create a socket for the ServerManager to listen for incoming connections
void	ServerManager::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_ServerAddress.sin_family , SOCK_STREAM, 0);
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
	if (bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_ServerAddress),
		sizeof(this->_ServerAddress)) != 0)
		throw ServerSocketException();
	this->_status = listen(this->_listenFd, BACKLOG);
	if (this->_status != 0)
		throw ServerSocketException();
}

// Keep accepting connections while the ServerManager is running
int	ServerManager::run(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;
	int						clientFd;
	
	addr_size = sizeof(client_addr);
	clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (clientFd == -1)
		throw ClientSocketException();
	this->addToPollFds(clientFd);
	std::cout << std::endl;
	printTimestamp();
	std::cout << YELLOW << "Server" << RESET << " accepted new connection on client socket " << clientFd << std::endl;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return clientFd;
}

void	ServerManager::preparePoll(void)
{
	this->_pollSize = 5;
	this->_pollFds = new struct pollfd[5];
	if (!this->_pollFds)
		std::cerr << "The problem is here." << std::endl;
	return;
}

void	ServerManager::setUpPoll(void)
{
	this->_pollFds[0].fd = this->_listenFd;
	this->_pollFds[0].events = POLLIN;
	this->_pollCount = 1;

	while (true)
	{
		this->_status = poll(this->_pollFds, this->_pollCount, 2000);
		if (this->_status == -1) {
			std::cerr << RED << "[Server] Poll error: " << std::strerror(errno) << std::endl;
			throw ServerSocketException();
		}
		else if (this->_status == 0)
			continue;
		for (int i = 0; i < this->_pollCount; i++)
		{
			if ((this->_pollFds[i].revents & POLLIN))
			{
				if (this->_pollFds[i].fd == this->_listenFd)
					this->run();
				else
					this->handleClientConnection(this->_pollFds[i].fd);
			}
			else if (this->_pollFds[i].revents & (POLLHUP | POLLERR))
			{
				printTimestamp();
				std::cout << RED << "Closing " << RESET << "client socket " << RESET << this->_pollFds[i].fd << std::endl;
				close(this->_pollFds[i].fd);
				this->delFromPollFds(i);
			}
		}
	}
}

void	ServerManager::addToPollFds(int clientFd)
{
	if (this->_pollCount == this->_pollSize)
	{
		this->_pollSize *= 2;
	}
	this->_pollFds[this->_pollCount].fd = clientFd;
	this->_pollFds[this->_pollCount].events = POLLIN;
	this->_pollCount++;

}

void	ServerManager::delFromPollFds(int i)
{
	this->_pollFds[i] = this->_pollFds[i - 1];
	this->_pollCount--;
}

const char*	ServerManager::ServerSocketException::what(void) const throw()
{
	return ("Server socket: ");
}

const char*	ServerManager::ClientSocketException::what(void) const throw()
{
	return ("Client socket: ");
}
