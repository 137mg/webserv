/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/15 14:55:26 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{	
	this->_port = 0;
	this->_serverName = "";
	this->_root = "";
	this->_index = "";
	this->_listenFd = 0;
	this->_clientMaxBodySize = 0;
	this->_autoIndex = false;
	return;
}

Server::~Server(void)
{
	std::cout << "Closing server socket." << std::endl;
	close(this->_listenFd);
	return;
}

// Step 1: create a socket
void	Server::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_serverAddress.sin_family , SOCK_STREAM, 0);
	if (this->_listenFd == -1) {
		std::cerr << "Socket fd error = " << std::strerror(errno) << std::endl;
		return;
	}

	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
    if (setsockopt(this->_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
        std::cerr << "setsockopt error: " << std::strerror(errno) << std::endl;
        close(this->_listenFd);
        return;
    }
	std::cout << "Created server socket fd = " << this->_listenFd << std::endl;
}

// Step 2: Identify a socket
void	Server::bindSocket(void)
{
	int	status;
	
	status = bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return ;
	}
	std::cout << "Bound socket to localhost port " << PORT << std::endl;

	// Step 3: Wait for incoming connection
	std::cout << "Listening on port " << PORT << std::endl;
	status = listen(this->_listenFd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return ;
	}
}

void	Server::config(void)
{

	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(PORT);

	this->createSocket();
	this->bindSocket();
}

void	Server::acceptConnection(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;

	
	addr_size = sizeof(client_addr);
	this->_clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (this->_clientFd == -1) {
		std::cerr << "client fd error: " << std::strerror(errno) << std::endl;
		exit(1);
	}
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Accepted new connection on client socket fd: " << this->_clientFd << std::endl;
	this->processConnection();
}

void	Server::processConnection(void)
{
	char	buffer[BUFSIZ];
	int		bytes_read;
	
	bytes_read = 1;
	while (bytes_read >= 0)
	{
		std::cout << "Reading client socket " << this->_clientFd << std::endl;
		bytes_read  = recv(this->_clientFd, buffer, BUFSIZ, 0);
		if (bytes_read == 0) {
			std::cout << "Client socket " << this->_clientFd << " closed the connection." << std::endl;
			break;
		}
		else if (bytes_read == -1) {
			std::cerr << "recv error " << std::strerror(errno) << std::endl;
			break;
		}
		else
			this->handleRequest(buffer);

	}
	std::cout << "Closing client socket." << std::endl;
	// close(this->_clientFd);
	std::cout << "------------------------------------------------" << std::endl;
}

void	Server::run(void)
{
	while (1)
	{
		this->acceptConnection();
	}
	return;
}
