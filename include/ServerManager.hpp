/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:47:12 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/06 11:51:19 by psadeghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>
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
		std::vector<int>						_listenFds;
		//int										_listenFds;
		int										_clientFd;
		struct sockaddr_in						_ServerAddress;
		std::string								_buffer;
		size_t									_requestSize;

		// struct pollfd							*_pollFds;
		std::vector<pollfd>						_pollFdsVector;
		
		std::map<int, std::string>				_clientBuffers;
		int										_status;

	public:
		ServerManager(void);
		~ServerManager(void);
	
		int		newClientConnection(int listenFd);
		void	config(void);
		void	configFile(const char* filename);
		int		createSocket(void);
		void	bindSocket(int sockfd);

		void	setUpPoll(void);
		void	addToPollFds(int clientFd);
		void	delFromPollFds(int i);

		bool	handleClientConnection(int clientFd);
		bool	isRequestComplete(const std::string &request_buffer);

		size_t	getRequestSize(std::string request_buffer);

		void	selectServer(std::string buffer, int clientFd);
		
		void	closeClientConnection(unsigned long i);
		void	monitorSockets(void);
		void	handleSocketEvents(void);

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

void	serverMessage(const std::string &message, int clientFd, std::string color);
void	clientMessage(const std::string &message, int clientFd);
void	printTimestamp(void);

#define MESSAGE_BUFFER 8192
#define MB 1048576
#define DEFAULT_PATH "config_files/default.toml"

#endif
