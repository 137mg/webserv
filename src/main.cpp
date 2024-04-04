/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/02/15 18:07:06 by psadeghi      #+#    #+#                 */
/*   Updated: 2024/03/25 14:03:39 by parisasadeq   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct addrinfo hints = {};

	struct addrinfo *res;

	struct addrinfo *r;

	int status;

	char buffer[INET6_ADDRSTRLEN];

	if (argc != 2) {
		std::cerr << "usage: /a.out hostname." << std::endl;
		return 1;
	}

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(argv[1], "http", &hints, &res);
	//status = getaddrinfo(argv[1], 0, &hints, &res);
	if (status != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return 2;
	}

	int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd  == -1) {
		std::cerr << "socket couldn't do it's job!" << std::endl;
		return 3;
	}

	if (connect(socket_fd, res->ai_addr, res->ai_addrlen) == -1) {
		std::cerr << "connect failed!" << std::endl;
		close(socket_fd); // Close the socket in case of failure
		return 4;
	}
	std::cout << "IP adresses for " << argv[1] << std::endl;

	r = res;

	while (r != nullptr) {
		//void *addr;
		if (r->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(r->ai_addr);
			inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer);
			std::cout << "IPv4: " << buffer << std::endl;
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(r->ai_addr);
			inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof buffer);
			std::cout << "IPv6: " << buffer << std::endl;
		}
		r = r->ai_next; // Next address in getaddrinfo()'s results
	}
	freeaddrinfo(res); // Free memory
	return 0;
}
