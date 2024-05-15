/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/15 16:58:57 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{	
	this->_port = PORT;
	this->_serverName = "Webserv";
	this->_root = "";
	this->_index = "";
	this->_listenFd = -1;
	this->_clientMaxBodySize = 1;
	this->_autoIndex = false;
}

Server::~Server(void)
{
	if (this->_listenFd != -1)
		close(this->_listenFd);
}

Server::Server(const Server &other)
{
	*this = other;
}

Server & Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->_port = other._port;
		this->_listenFd = other._listenFd;
		this->_clientFd = other._clientFd;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_serverName = other._serverName;
		this->_root = other._root;
		this->_autoIndex = other._autoIndex;
		this->_serverAddress = other._serverAddress;
	}
	return *this;
}

void	Server::config(void)
{
	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(PORT);

	this->createSocket();
	this->bindSocket();
	printTimestamp();
	std::cout << PURPLE << UNDER << this->_serverName << RESET << " up and running. Listening on port: "
		<< UNDER << this->_port << RESET << std::endl;
	std::cout << "-----------------------------------------------------------------------" << std::endl;
}

// Create a socket for the server to listen for incoming connections
void	Server::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_serverAddress.sin_family , SOCK_STREAM, 0);
	if (this->_listenFd == -1)
		throw ServerSocketException();
	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
    if (setsockopt(this->_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw ServerSocketException();
}

// Bind the socket to a specific address and port and listen for incoming connections
void	Server::bindSocket(void)
{	
	if (bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_serverAddress),
		sizeof(this->_serverAddress)) != 0)
		throw ServerSocketException();

	if (listen(this->_listenFd, BACKLOG) != 0)
		throw ServerSocketException();
}

// Keep accepting connections while the server is running
void	Server::run(void)
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

const char*	Server::ConfigFileException::what(void) const throw()
{
	return ("Configuration file: ");
}

const char*	Server::ServerSocketException::what(void) const throw()
{
	return ("Server socket: ");
}

const char*	Server::ClientSocketException::what(void) const throw()
{
	return ("Client socket: ");
}
