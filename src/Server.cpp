/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/09 14:00:18 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{	
	return;
}

void	Server::config(void)
{
	int	status;

	this->_address.sin_family = AF_INET;
	this->_address.sin_addr.s_addr = INADDR_ANY;
	this->_address.sin_port = htons(PORT);

	// Step 1: create a socket
	// sa.sin_family or AF_INET
	this->_socketFd = socket(this->_address.sin_family , SOCK_STREAM, 0);
	if (this->_socketFd == -1) {
		std::cerr << "Socket fd error = " << std::strerror(errno) << std::endl;
		return;
	}
	std::cout << "Created server socket fd = " << this->_socketFd << std::endl;
	//

	// Step 2: Identify a socket
	status = bind(this->_socketFd, reinterpret_cast<struct  sockaddr*>(&this->_address), sizeof(this->_address));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return ;
	}
	std::cout << "Bound socket to localhost port " << PORT << std::endl;
	//

	// Step 3: Wait for incoming connection
	std::cout << "Listening on port " << PORT << std::endl;
	status = listen(this->_socketFd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return ;
	}
}

Server::~Server(void)
{
	std::cout << "Closing server socket." << std::endl;
	close(this->_socketFd);
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
		client_fd = accept(this->_socketFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
		if (client_fd == -1) {
			std::cerr << "client fd error: " << std::strerror(errno) << std::endl;
			return ; 
		}
		std::cout << "------------------------------------------------" << std::endl;
		std::cout << "Accepted new connection on client socket fd: " << client_fd << std::endl;
		//

		// Step 4: Send and receive messages
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
			else {
				std::string msg = "Got your message.";
				int msg_len = msg.length();
				int bytes_send;

				buffer[bytes_read] = '\0';

				std::cout << GREEN << "Message received from client socket " << client_fd << ": " << RESET << buffer << std::endl;
				// this is a problem
				bytes_send = send(client_fd, "Got your message.", msg_len, 0);
				if (bytes_send == -1) {
					std::cerr << "send error " << std::strerror(errno) << std::endl;
				}
				else if (bytes_send == msg_len) {
					std::cout << GREEN << "Sent full messsage to client socket " << client_fd << ": " << RESET << msg << std::endl;
				}
				else {
					std::cout << GREEN << "Sent partial message to client socket " << this->_socketFd << ": " << bytes_send << " bytes sent." << RESET << std::endl;
				}
			}
			std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			std::cout << hello << std::endl;
		}

		std::cout << "Closing client socket." << std::endl;
		close(client_fd);
		std::cout << "------------------------------------------------" << std::endl;
	}
	return;
}