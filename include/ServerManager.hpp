/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:47:12 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/31 15:13:04 by mgoedkoo         ###   ########.fr       */
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
#include <deque>
#include <map>
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>

#include "colors.h"
#include "Server.hpp"

class	Server;

class	ServerManager
{
	private:
		std::vector<uint16_t>					_ports;
		std::map<uint16_t, std::vector<Server>>	_serverMap;

		std::string								_ServerName;
		int										_listenFd;
		int										_clientFd;
		struct sockaddr_in						_ServerAddress;
		std::string								_buffer;
		size_t									_requestSize;

		struct pollfd							*_pollFds;
		int										_pollSize;
		int 									_pollCount;
		
		std::map<int, std::string>				_clientBuffers;
		int										_status;

	public:
		ServerManager(void);
		~ServerManager(void);
	
		int		run(void);
		void	config(void);
		void	configFile(const char* filename);
		void	handleRequest(std::string buffer, int clientFd);
		void	createSocket(void);
		void	bindSocket(void);
		void	getRequest(std::string file, int clientFd);
		void	deleteRequest(std::string file, int clientFd);
		void	postRequest(std::string buffer, std::string method, int clientFd);
		void	terminalMessage(const std::string &s1, const std::string &s2, int clientFd);

		void	preparePoll(void);
		void	setUpPoll(void);
		void	addToPollFds(int clientFd);
		void	delFromPollFds(int i);

		bool	handleClientConnection(int clientFd);
		bool	fileAccess(const std::string &path);
		bool	isRequestComplete(const std::string &request_buffer);

		int		readFromSocket(std::string &outbuffer, int clientFd);
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

#define MESSAGE_BUFFER 8192
#define MB 1048576
#define DEFAULT_PATH "config_files/default.toml"

#endif
