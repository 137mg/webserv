/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverConection.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 11:46:19 by parisasadeq       #+#    #+#             */
/*   Updated: 2024/02/20 16:32:20 by psadeghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <cerrno>
#include <cstring>

#define PORT 4242
#define BACKLOG 10

int main ( void ) {
	std::cout << "             SERVER SIDE             " << std::endl;

	struct sockaddr_in	sa = {};
	int	socket_fd;
	int	client_fd;
	int	status;

	struct sockaddr_storage	client_addr;
	socklen_t	addr_size;
	char	buffer[BUFSIZ];
	int		bytes_read;

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(PORT);

	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cerr << "Socket fd error = " << std::strerror(errno) << std::endl;
		return 1;
	}

	std::cout << "Created server socket fd = " << socket_fd << std::endl;

	status = bind(socket_fd, reinterpret_cast<struct  sockaddr*>(&sa), sizeof(sa));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return 2;
	}
	std::cout << "Bound socket to localhost port " << PORT << std::endl;

	std::cout << "Listening on port " << PORT << std::endl;
	status = listen(socket_fd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return 3;
	}

	addr_size = sizeof(client_addr);
	client_fd = accept(socket_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (client_fd == -1) {
		std::cerr << "client fd error: " << std::strerror(errno) << std::endl;
		return 4; 
	}
	std::cout << "Accepted new connection on client socket fd: " << client_fd << std::endl;

	bytes_read = 1;
	while (bytes_read >= 0) {
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

			std::cout << "Message received from client socket " << client_fd << ": " << buffer << std::endl;
			// this is a problem
			bytes_send = send(client_fd, "Got your message.", msg_len, 0);
			if (bytes_send == -1) {
				std::cerr << "send error " << std::strerror(errno) << std::endl;
			}
			else if (bytes_send == msg_len) {
				std::cout << "Sent full messsage to client socket " << client_fd << ": " << msg << std::endl;
			}
			else {
				std::cout << "Sent partial message to client socket " << socket_fd << ": " << bytes_send << " bytes sent." << std::endl;
			}
		}
	}

	std::cout << "Closing client socket." << std::endl;
	close(client_fd);
	std::cout << "Closing server socket." << std::endl;
	close(socket_fd);

	return 0;
}