/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:11 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/04 15:35:12 by Julia         ########   odam.nl         */
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
		std::string			_buffer;
		// location && error pages
		
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server & operator=(const Server &other);
	
		void	run(void);
		void	config(void);
		void	handleRequest(std::string, int bytesRead);
		void	createSocket(void);
		void	bindSocket(void);
		void	handleClientConnection(void);
		void	getRequest(std::string file);
		void	postRequest(std::string buffer, int bytesRead);
		void	terminalMessage(const std::string &s1, const std::string &s2);
		int		processRequest(const std::string &request_buffer);

		bool	fileAccess(const std::string &path);
		bool	isRequestComplete(const std::string &request_buffer);

		int		readFromSocket(std::string &outBuffer);

		std::string	serveFile(const std::string &path, const std::string &status, const std::string &color);
		std::string	parseRequest(const std::string &request);
		std::string	getHeader(std::string buffer, std::string key);
		
};

void	printTimestamp(void);
#define MESSAGE_BUFFER 40000

#endif
