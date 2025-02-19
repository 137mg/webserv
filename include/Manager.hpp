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
# define MANAGER_HPP

# include <netinet/in.h>
# include <netdb.h>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <sstream>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <csignal>
# include <ctime>
# include <cstring>
# include <string>
# include <cerrno>
# include <vector>
# include <deque>
# include <map>
# include <algorithm>
# include <dirent.h>
# include <fcntl.h>
# include <poll.h>

# include "Colors.h"
# include "Structs.hpp"
# include "Server.hpp"

# define READING 0
# define WRITING 1
# define MESSAGE_BUFFER 8192
# define MB 1048576

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
		
		std::map<int, std::string>				_clientResponses;
		std::map<int, std::string>				_clientErrorResponses;
		std::map<int, std::string>				_clientBuffers;
		int										_status;

		std::map<int, time_t> 					_clientActivityMap;
		std::map<int, int>						_clientStatus;
		int										_timeout;

	public:
		Manager(void);
		~Manager(void);
	
		void						configFile(const char* filename);
		void						startServer(void);
		int							createSocket(void);
		void						bindSocket(int sockfd);
		void						setUpPoll(int listenFd);
		void						addToPollFds(int clientFd, short events);
		void						delFromPollFdsByValue(int fd);

		bool						readRequest(int clientFd);
		bool						isRequestTooLarge(std::string buffer);
		bool						isRequestComplete(std::string buffer);
		bool						handleChunkedRequest(std::string& buffer, int clientFd);

		std::string					getValue(std::string request, std::string key);
		uint16_t					findPort(int clientFd);
		t_header					parseHeader(std::string request, int clientFd);
		void						selectServer(std::string buffer, int clientFd);
		
		int							newClientConnection(int listenFd);
		void						closeClientConnection(int clientFd);
		void						monitorSockets(void);
		void						handleSocketEvents(void);
		void						checkForTimeouts(void);

		void						addCGIProcess(t_CGIProcess cgiProcess);
		void						removeCGIProcess(int fd);
		bool						isCGIOutputFd(int fd);
		bool						isCGIInputFd(int fd);
		void						handleCGIOutput(int cgiFd);
		void						handleCGIInput(int cgiFd);
		t_CGIProcess				&getCGIProcessForFd(int fd);
		void						handleHangup(int cgiFd);
	
		void						sendResponse(std::string response, int clientFd);
		bool						sendErrorResponse(std::string response, int clientFd);
		void						sendPendingResponse(int clientFd);
		void						markFdForWriting(int clientFd);
		void						markFdForReading(int clientFd);

		void						setClientStatus(int fd, int status);
		void						setClientResponses(int clientFd, std::string response);
		void						setClientErrorResponses(int clientFd, std::string response);
	

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

#endif
