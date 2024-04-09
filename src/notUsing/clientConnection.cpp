/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   clientConnection.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/02/20 16:25:57 by psadeghi      #+#    #+#                 */
/*   Updated: 2024/04/05 16:10:20 by juvan-to      ########   odam.nl         */
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
#include "colors.h"

int main (int argc, char **argv)
{
	std::cout << "             CLIENT SIDE             " << std::endl;

	struct sockaddr_in	sa = {};
	int socket_fd;
	int status;
	char buffer[BUFSIZ];
	int bytes_read;
	std::string msg;
	int msg_len;
	int bytes_sent;

	if (argc != 2) {
		std::cout << "Please put the message you want to send as an argument." << std::endl;
		return 1;
	}

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sa.sin_port = htons(PORT);

	socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		std::cerr << "socket fd error: " << std::strerror(errno) << std::endl;
		return 2;
	}
	std::cout << "Created socket fd: " << socket_fd << std::endl;

	status = connect(socket_fd, reinterpret_cast<struct sockaddr *>(&sa), sizeof(sa));
	if (status != 0) {
		std::cerr << "Connect error: " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "Connected socket to localhost port " << PORT << std::endl;

	msg = argv[1];
	msg_len = msg.size();
	bytes_sent = send(socket_fd, argv[1], msg_len, 0);
	if (bytes_sent == -1) {
		std::cerr << "send error: " << std::strerror(errno) << std::endl;
		
	}
	else if (bytes_sent == msg_len) {
		std::cout << GREEN << "Sent full message: " << RESET << msg << std::endl;
	}
	else {
		std::cout << GREEN << "Sent the message partialy." << RESET << std::endl;
	}


	bytes_read = 1;
	while (bytes_read >= 0) {
		bytes_read = recv(socket_fd, buffer, BUFSIZ, 0);
		if (bytes_read == 0) {
			std::cout << "Server closed the connection." << std::endl;
			break;
		}
		else if (bytes_read == -1) {
			std::cerr << "recv error: " << std::strerror(errno) << std::endl;
			break;
		}
		else {
			buffer[bytes_read] = '\0';
			std::cout << GREEN << "Message received = " << RESET << buffer << std::endl;
			break;
		}
	}
	std::cout << "Closing socket." << std::endl;
	close(socket_fd);
	return 0;
}