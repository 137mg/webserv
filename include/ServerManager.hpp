/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:47:12 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/22 16:27:21 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

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
#include <map>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>

#include "colors.h"

class ServerManager
{
	private:
		uint16_t			_port;
		int					_listenFd;
		int					_clientFd;
		int					_clientMaxBodySize;
		std::string			_ServerName;
		std::string			_root;
		std::string			_index;
		bool				_autoIndex;
		struct sockaddr_in	_ServerAddress;
		std::string			_buffer;
		size_t				_requestSize;
		// location && error pages
		
	public:
		ServerManager();
		~ServerManager();
		ServerManager(const ServerManager &other);
		ServerManager & operator=(const ServerManager &other);
	
		void	run(void);
		void	config(void);
		void	configFile(const char* filename);
		void	handleRequest(std::string);
		void	createSocket(void);
		void	bindSocket(void);
		void	getRequest(std::string file);
		void	deleteRequest(std::string file);
		void	postRequest(std::string buffer, std::string method);
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
#define MB 1048576
#define DEFAULT_PATH "config_files/default.conf"

#endif
