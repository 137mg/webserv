/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 11:46:19 by parisasadeq       #+#    #+#             */
/*   Updated: 2024/02/20 14:15:24 by psadeghi         ###   ########.fr       */
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

#define PORT "4242"
#define BACKLOG 10

int main (void)
{
	struct addrinfo hints = {};
	struct addrinfo *res;
	int socket_fd;
	int client_fd;
	int status;

	struct sockaddr_storage client_addr;
	socklen_t addr_size;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	status = getaddrinfo(nullptr, PORT, &hints, &res);
	if (status != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return 1;
	}
	
	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	status = bind(socket_fd, res->ai_addr, res->ai_addrlen);
	if (status != 0) {
		std::cerr << "bind: " << std::strerror(errno) << std::endl; 
		return 2;
	}
	listen(socket_fd, BACKLOG);

	addr_size = sizeof client_addr;
	client_fd = accept(socket_fd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (client_fd == -1) {
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		return 3;
	}
	std::cout << "New connection! Socket fd: " << socket_fd << ", client fd: " << client_fd << std::endl;
	
	return 0;
}