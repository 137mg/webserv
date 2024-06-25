/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Manager.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/16 15:47:12 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/19 01:18:04 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGER_HPP
#define MANAGER_HPP

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

#include <csignal>

#include "Colors.h"
#include "structs.hpp"
#include "Server.hpp"

#define READING 0
#define WRITING 1

extern volatile bool RUNNING;

class	Server;

class	Manager
{
	private:
		std::vector<uint16_t>					_ports;
		std::map<uint16_t, std::vector<Server>>	_serverMap;
		std::map<int, int>						_fdMap;

		std::string								_ServerName;
		std::vector<int>						_listenFds;
		int										_clientFd;
		struct sockaddr_in						_ServerAddress;
		std::string								_buffer;
		size_t									_requestSize;

		std::vector<pollfd>						_pollFdsVector;
		std::vector<t_CGIProcess>				_cgiProcesses;
		
		std::map<int, std::string>				_clientBuffers;
		int										_status;

		std::map<int, time_t> 					_clientActivityMap;
		std::map<int, int>						_clientStatus;
		const int								_timeout;

	public:
		Manager(void);
		~Manager(void);
	
		std::map<int, std::string>				clientResponses;
		std::map<int, std::string>				clientErrorResponses;
	
		int		newClientConnection(int listenFd);
		void	config(void);
		void	configFile(const char* filename);
		int		createSocket(void);
		void	bindSocket(int sockfd);

		void	setUpPoll(int listenFd);
		void	addToPollFds(int clientFd);

		bool	readRequest(int clientFd);
		bool	isRequestTooLarge(std::string buffer);
		bool	isRequestComplete(std::string buffer);
		bool	handleChunkedRequest(std::string& buffer, int clientFd);

		std::string	getValue(std::string request, std::string key);
		uint16_t	findPort(int clientFd);
		t_header	parseHeader(std::string request, int clientFd);
		void		selectServer(std::string buffer, int clientFd);
		
		void	closeClientConnection(int clientFd);
		void	monitorSockets(void);
		void	handleSocketEvents(void);
		void	checkForTimeouts(void);

		void	addCGIProcess(t_CGIProcess cgiProcess);
		bool	checkIfCGIProcessExistsForFd(int fd);

		// imma try some things here hear me out
		void				handleCGIOutput(int cgiFd, size_t pollIndex);
		void				handleCGIInput(int cgiFd, size_t pollIndex);
		t_CGIProcess		&getCGIProcessForFd(int fd);
		void				sendResponse(std::string response, int clientFd);
		void				sendPendingResponse(int clientFd);
		void				markFdForWriting(int clientFd);
		void				clearFdForWriting(int clientFd);
		bool				isCGIInputFd(int fd);
		void				removeCGIProcess(int fd);
		void				delFromPollFdsByValue(int fd);
		bool				sendErrorResponse(std::string response, int clientFd);
		std::map<int, int>	getClientStatus(void);
		void				setClientStatus(int fd, int status);
	
		//void	signalHandler(int signum);

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
