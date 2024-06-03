/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerManager.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/16 15:47:12 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/03 15:45:10 by juvan-to      ########   odam.nl         */
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
		void	createSocket(void);
		void	bindSocket(void);

		void	setUpPoll(void);
		void	addToPollFds(int clientFd);
		void	delFromPollFds(int i);

		bool	handleClientConnection(int clientFd);
		bool	isRequestComplete(const std::string &request_buffer);

		int		readFromSocket(std::string &outbuffer, int clientFd);
		size_t	getRequestSize(std::string request_buffer);

		void	selectServer(std::string buffer, int clientFd);

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

void	terminalMessage(const std::string &s1, const std::string &s2, int clientFd);
void	printTimestamp(void);

#define MESSAGE_BUFFER 8192
#define MB 1048576
#define DEFAULT_PATH "config_files/default.toml"

#endif
