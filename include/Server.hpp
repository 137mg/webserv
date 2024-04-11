/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:11 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/11 17:27:35 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <vector>
#include <sstream>
#include "colors.h"

class Server
{
	private:
		uint16_t			_port;
		int					_listenFd;
		int					_clientMaxBodySize;
		std::string			_serverName;
		std::string			_root;
		std::string			_index;
		bool				_autoIndex;
		struct sockaddr_in	_serverAddress;
		// location && error pages
		
	public:
		Server();
		~Server();
	
		void	run(void);
		void	config(void);
		void	handleRequest(char *buffer);
		void	createSocket(void);
		void	bindSocket(void);
		void	acceptConnection(void);
		void	processConnection(int client_fd);
		
};

#endif