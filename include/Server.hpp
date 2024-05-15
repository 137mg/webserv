/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:11 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/15 16:16:27 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cerrno>
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>

#include "colors.h"

class Server
{
	private:
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
		size_t				_requestSize;
		// location && error pages
		
	public:
		Server();
		~Server();
		Server(const Server &other);
		Server & operator=(const Server &other);
	
		void	run(void);
		void	config(void);
		void	configFile(char* filename);
		void	handleRequest(std::string);
		void	createSocket(void);
		void	bindSocket(void);
		void	getRequest(std::string file);
		void	deleteRequest(std::string file);
		void	postRequest(std::string buffer);
		void	terminalMessage(const std::string &s1, const std::string &s2);

		bool	handleClientConnection(void);
		bool	fileAccess(const std::string &path);
		bool	isRequestComplete(const std::string &request_buffer);

		int		readFromSocket(std::string &outBuffer);
		size_t	getRequestSize(std::string request_buffer);

		std::string	showUploads(const std::string &path, const std::string &status, const std::string &color);
		std::string	serveFile(const std::string &path, const std::string &status, const std::string &color);
		std::string	parseRequest(const std::string &request);
		std::string	getHeader(std::string buffer, std::string key);

		class	ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class	ServerSocketException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class	ClientSocketException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

void	printTimestamp(void);

#define MESSAGE_BUFFER 40000
#define PORT 8080

#endif
