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
	this->_ServerName = "Webserv";
	this->_pollFds = new struct pollfd[5];
	this->_pollCount = 5;
	this->_pollSize = 0;
}

ServerManager::~ServerManager(void)
{
	delete this->_pollFds;
	for (std::vector<int>::size_type i = 0; i < this->_listenFds.size(); i++)
	{
		if (this->_listenFds[i] != -1)
			close(this->_listenFds[i]);
	}
}

// uses the first port in the list of ports now, needs to change once we add multiple ports!
void	ServerManager::config(void)
{
	for (std::vector<uint16_t>::size_type i = 0; i != _ports.size(); i++) {
		this->_ServerAddress.sin_family = AF_INET;
		this->_ServerAddress.sin_addr.s_addr = INADDR_ANY;
		this->_ServerAddress.sin_port = htons(_ports[i]);

		int sockfd = createSocket();
		this->bindSocket(sockfd);
		this->_listenFds.push_back(sockfd);
		printTimestamp();
		std::cout << PURPLE << UNDER << this->_ServerName << RESET << " up and running. Listening on port: "
			<< UNDER << _ports[i] << RESET << std::endl;
		std::cout << "-----------------------------------------------------------------------" << std::endl;
	}
}
 
// Create a socket for the ServerManager to listen for incoming connections
int	ServerManager::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	int listenFd = socket(this->_ServerAddress.sin_family , SOCK_STREAM, 0);
	if (listenFd == -1)
		throw ServerSocketException();
	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw ServerSocketException();
	return listenFd;
}

// Bind the socket to a specific address and port and listen for incoming connections
void	ServerManager::bindSocket(int sockfd)
{	
	if (bind(sockfd, reinterpret_cast<struct  sockaddr*>(&this->_ServerAddress),
		sizeof(this->_ServerAddress)) != 0)
		throw ServerSocketException();
	int status = listen(sockfd, BACKLOG);
	if (status != 0)
		throw ServerSocketException();
}

// Keep accepting connections while the ServerManager is running
int	ServerManager::run(int listenFd)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;
	int						clientFd;
	
	addr_size = sizeof(client_addr);
	clientFd = accept(listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (clientFd == -1)
		throw ClientSocketException();
	this->addToPollFds(clientFd);
	std::cout << std::endl;
	printTimestamp();
	std::cout << YELLOW << "Server" << RESET << " accepted new connection on client socket " << clientFd << std::endl;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return clientFd;
}

void	ServerManager::setUpPoll(void)
{	
	this->_pollCount = _listenFds.size();
	for (size_t i = 0; i < _listenFds.size(); ++i) {
		this->_pollFds[i].fd = _listenFds[i];
		this->_pollFds[i].events = POLLIN;
	}

	while (true)
	{
		this->_status = poll(this->_pollFds, this->_pollCount, 2000);
		if (this->_status == -1)
			throw ServerSocketException();
		else if (this->_status == 0)
			continue;
		for (int i = 0; i < this->_pollCount; i++)
		{
			if ((this->_pollFds[i].revents & POLLIN))
			{
				if (std::find(_listenFds.begin(), _listenFds.end(), this->_pollFds[i].fd) != _listenFds.end()) 
					this->run(this->_pollFds[i].fd);
				else
				{
					if (!this->handleClientConnection(this->_pollFds[i].fd))
					{
						this->delFromPollFds(i);
						break;
					}
				}
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
