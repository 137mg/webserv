/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManagerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 15:46:03 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 15:46:06 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

Manager::Manager(void) : _timeout(60)
{
	_ServerName = "Webserv";
	_clientFd = -1;
	_requestSize = 0;
	_status = 0;
}

Manager::~Manager(void)
{
	for (std::vector<int>::size_type i = 0; i < _listenFds.size(); i++)
	{
		if (_listenFds[i] != -1)
			close(_listenFds[i]);
	}
}

void	Manager::startServer(void)
{
	for (size_t i = 0; i != _ports.size(); i++)
	{
		_ServerAddress.sin_family = AF_INET;
		_ServerAddress.sin_addr.s_addr = INADDR_ANY;
		_ServerAddress.sin_port = htons(_ports[i]);

		int sockfd = createSocket();
		bindSocket(sockfd);
		_listenFds.push_back(sockfd);
		setUpPoll(sockfd);
		printTimestamp();
		std::cout << PURPLE << UNDER << _ServerName << RESET << " up and running. Listening on port: "
			<< UNDER << _ports[i] << RESET << std::endl;
		std::cout << "-----------------------------------------------------------------------" << std::endl;
	}
}

// Create a socket for the Manager to listen for incoming connections
int	Manager::createSocket(void)
{
	int	opt;
	int	listenFd;

	listenFd = socket(_ServerAddress.sin_family , SOCK_STREAM, 0);
	if (listenFd == -1)
		throw ServerSocketException();
	opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw ServerSocketException();
	return listenFd;
}

// Bind the socket to a specific address and port and listen for incoming connections
void	Manager::bindSocket(int sockfd)
{	
	if (bind(sockfd, reinterpret_cast<struct  sockaddr*>(&_ServerAddress),
		sizeof(_ServerAddress)) != 0)
		throw ServerSocketException();
	if (listen(sockfd, BACKLOG) != 0)
		throw ServerSocketException();
}

void	Manager::setUpPoll(int listenFd)
{
	pollfd	pfd;

	pfd.fd = listenFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFdsVector.push_back(pfd);
}

void	Manager::setClientResponses(int clientFd, std::string response)
{
	_clientResponses[clientFd] = response;
}

void	Manager::setClientErrorResponses(int clientFd, std::string response)
{
	_clientErrorResponses[clientFd] = response;
}

void	Manager::setClientStatus(int fd, int status)
{
	_clientStatus[fd] = status;
}

const char*	Manager::ServerSocketException::what(void) const throw()
{
	return ("Server socket: ");
}

const char*	Manager::ClientSocketException::what(void) const throw()
{
	return ("Client socket: ");
}
