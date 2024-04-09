/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:11 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/09 13:57:36 by juvan-to      ########   odam.nl         */
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
#include "colors.h"

class Server
{
	private:
		int					_socketFd;
		struct sockaddr_in	_address;
		
	public:
		Server();
		~Server();
	
		void	run(void);
		void	config(void);
		
};

#endif