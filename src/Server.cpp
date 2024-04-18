/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/18 15:13:30 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{	
	this->_port = PORT;
	this->_serverName = "Webserv";
	this->_root = "";
	this->_index = "";
	this->_listenFd = 0;
	this->_clientMaxBodySize = 1;
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

	// Step 3: Wait for incoming connection
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
	
	printTimestamp();
	std::cout << this->_serverName << " up and running. Listening on port: "
		<< this->_port << std::endl;
	
}

void	Server::acceptConnection(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;

	
	addr_size = sizeof(client_addr);
	this->_clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (this->_clientFd == -1) {
		std::cerr << RED << "client fd error: " << std::strerror(errno) << RESET << std::endl;
		exit(1);
	}

	fcntl(this->_clientFd, F_SETFL, O_NONBLOCK);
	this->processConnection();
}

void	Server::processConnection(void)
{
	char			buffer[BUFSIZ];
	struct pollfd	fds[this->_clientMaxBodySize + 1];
	int				bytes_read;
	int				pollResult;

	fds[0].fd = this->_clientFd;
	fds[0].events = POLLIN | POLLHUP | POLLERR;
	bytes_read = 1;
	while (1)
	{
		pollResult = poll(fds, 1, -1);
		if (pollResult == -1)
		{
			std::cerr << RED << BOLD << "poll error " << std::strerror(errno) << RESET << std::endl;
			break;
		}
		if (fds[0].revents && POLLIN)
		{
			bytes_read  = read(this->_clientFd, buffer, BUFSIZ);
			if (bytes_read == 0)
			{
				std::cout << "Client socket " << this->_clientFd << " closed the connection." << std::endl;
				break;
			}
			else if (bytes_read == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;
				else
				{
					std::cerr << RED << BOLD << "read error " << std::strerror(errno) << RESET << std::endl;
					break;	
				}
			}
			else
				this->handleRequest(buffer);
		}
		// Check for hanging or errors
		if (fds[0].revents & (POLLHUP | POLLERR))
		{
			std::cerr << RED << BOLD << "Error or connection closed on socket " << this->_clientFd << RESET << std::endl;
			break;
		}


	}
	printTimestamp();
	std::cout << "Closing client socket. " << std::endl;
	close(this->_clientFd);
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
