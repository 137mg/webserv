/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:11 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/28 23:28:27 by Julia         ########   odam.nl         */
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
#include <fstream>
#include <fcntl.h>
#include <poll.h>

#include "colors.h"

class Server
{
	protected:
		uint16_t			_port;
		int					_listenFd;
		int					_clientFd;
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
		Server(const Server &other);
		Server & operator=(const Server &other);
	
		void	run(void);
		void	config(void);
		void	handleRequest(char *buffer, int bytesRead);
		void	createSocket(void);
		void	bindSocket(void);
		void	acceptConnection(void);
		void	processConnection(void);
		void	getRequest(std::string file);
		void	postRequest(std::string path, char *buffer, int bytesRead);
		void	terminalMessage(const std::string &s1, const std::string &s2);

		bool	fileAccess(const std::string &path);

		std::string	serveFile(const std::string &path, const std::string &status, const std::string &color);
		std::string	parseRequest(const std::string &request);
		std::string	getHeader(std::string buffer, std::string key);
		
};

void	printTimestamp(void);

#endif