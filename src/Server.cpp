/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/09 14:31:52 by juvan-to      ########   odam.nl         */
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

void	Server::config(void)
{
	int	status;

	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(PORT);

	// Step 1: create a socket
	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_serverAddress.sin_family , SOCK_STREAM, 0);
	if (this->_listenFd == -1) {
		std::cerr << "Socket fd error = " << std::strerror(errno) << std::endl;
		return;
	}
	std::cout << "Created server socket fd = " << this->_listenFd << std::endl;
	//

	// Step 2: Identify a socket
	status = bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return ;
	}
	std::cout << "Bound socket to localhost port " << PORT << std::endl;
	//

	// Step 3: Wait for incoming connection
	std::cout << "Listening on port " << PORT << std::endl;
	status = listen(this->_listenFd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return ;
	}
}

Server::~Server(void)
{
	std::cout << "Closing server socket." << std::endl;
	close(this->_listenFd);
	return;
}

void	Server::run(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;
	char					buffer[BUFSIZ];
	int						bytes_read;
	int						client_fd;

	while (1)
	{
		addr_size = sizeof(client_addr);
		client_fd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
		if (client_fd == -1) {
			std::cerr << "client fd error: " << std::strerror(errno) << std::endl;
			return ; 
		}
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << "Accepted new connection on client socket fd: " << client_fd << std::endl;

		bytes_read = 1;
		while (bytes_read >= 0)
		{
			std::cout << "Reading client socket " << client_fd << std::endl;
			bytes_read  = recv(client_fd, buffer, BUFSIZ, 0);
			if (bytes_read == 0) {
				std::cout << "Client socket " << client_fd << " closed the connection." << std::endl;
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
		close(client_fd);
		std::cout << "------------------------------------------------" << std::endl;
	}
	return;
}

// to be implemented, will handle the GET and POST requests
void	Server::handleRequest(std::string buffer)
{
	std::cout << "Handling request..." << std::endl;
	std::cout << buffer << std::endl;
	return;	
}
