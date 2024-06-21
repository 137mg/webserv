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
	this->_ServerName = "Webserv";
}

Manager::~Manager(void)
{
	for (std::vector<int>::size_type i = 0; i < this->_listenFds.size(); i++)
	{
		if (this->_listenFds[i] != -1)
			close(this->_listenFds[i]);
	}
}

void	Manager::config(void)
{
	for (size_t i = 0; i != _ports.size(); i++)
	{
		this->_ServerAddress.sin_family = AF_INET;
		this->_ServerAddress.sin_addr.s_addr = INADDR_ANY;
		this->_ServerAddress.sin_port = htons(_ports[i]);

		int sockfd = createSocket();
		this->bindSocket(sockfd);
		this->_listenFds.push_back(sockfd);
		printTimestamp();
		std::cout << PURPLE << UNDER << this->_ServerName << RESET << " up and running. Listening on port: "
			<< UNDER << _ports[i] << RESET << std::endl;
		std::cout << "-----------------------------------------------------------------------" << std::endl;
	}
}

// Create a socket for the Manager to listen for incoming connections
int	Manager::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	int listenFd = socket(this->_ServerAddress.sin_family , SOCK_STREAM, 0);
	if (listenFd == -1)
		throw ServerSocketException();
	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw ServerSocketException();
	return listenFd;
}

// Bind the socket to a specific address and port and listen for incoming connections
void	Manager::bindSocket(int sockfd)
{	
	if (bind(sockfd, reinterpret_cast<struct  sockaddr*>(&this->_ServerAddress),
		sizeof(this->_ServerAddress)) != 0)
		throw ServerSocketException();
	if (listen(sockfd, BACKLOG) != 0)
		throw ServerSocketException();
}

int	Manager::newClientConnection(int listenFd)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;
	int						clientFd;
	
	addr_size = sizeof(client_addr);
	clientFd = accept(listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (clientFd == -1)
		throw ClientSocketException();
	this->addToPollFds(clientFd);
	_clientActivityMap[clientFd] = std::time(nullptr); // Track the last activity time
	std::cout << std::endl;
	printTimestamp();
	std::cout << YELLOW << "Server" << RESET << " accepted new connection on client socket " << clientFd << std::endl;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return clientFd;
}

void	Manager::setUpPoll(void)
{	
	for (size_t i = 0; i < _listenFds.size(); ++i)
	{
		pollfd pfd;
		pfd.fd = _listenFds[i];
		pfd.events = POLLIN;
		_pollFdsVector.push_back(pfd);
	}
	this->monitorSockets();
}

void Manager::checkForTimeouts(void)
{
	time_t now = std::time(nullptr);
	for (unsigned long i = 0; i < this->_pollFdsVector.size(); i++)
	{
		int clientFd = this->_pollFdsVector[i].fd;
		if (_clientActivityMap.find(clientFd) != _clientActivityMap.end())
		{
			if (now - _clientActivityMap[clientFd] > this->_timeout)
			{
				std::cout << RED <<"Client " << clientFd << " 408 Request Timeout"  << RESET << std::endl;
				closeClientConnection(i);
				i--; // Adjust index after erasing the element
			}
		}
	}
}

void	Manager::monitorSockets(void)
{
	while (RUNNING)
	{
		this->_status = poll(this->_pollFdsVector.data(), this->_pollFdsVector.size(), 2000);
		if (this->_status == -1)
			throw ServerSocketException();
		else if (this->_status == 0) {
			checkForTimeouts();
			continue;
		}
		handleSocketEvents();	
		checkForTimeouts();
	}
}

void Manager::handleSocketEvents(void)
{
	for (unsigned long i = 0; i < this->_pollFdsVector.size() && RUNNING; i++)
	{
		if ((this->_pollFdsVector[i].revents & POLLIN))
		{
			if (checkIfCGIProcessExistsForFd(_pollFdsVector[i].fd))
			{
				handleCGIOutput(_pollFdsVector[i].fd, i);
				continue;
			}

			if (std::find(_listenFds.begin(), _listenFds.end(), this->_pollFdsVector[i].fd) != _listenFds.end()) 
				this->newClientConnection(this->_pollFdsVector[i].fd);
			else
			{
				if (!this->readRequest(this->_pollFdsVector[i].fd))
				{
					closeClientConnection(i);
					break;
				}
				_clientActivityMap[this->_pollFdsVector[i].fd] = std::time(nullptr); // Update the last activity time
			}
		}
		else if (this->_pollFdsVector[i].revents & POLLOUT)
        {
			if (isCGIInputFd(_pollFdsVector[i].fd))
            {
                handleCGIInput(_pollFdsVector[i].fd, i);
                continue;
            }
            // Handle outgoing data (response)
            sendPendingResponse(_pollFdsVector[i].fd);
        }
		else if (this->_pollFdsVector[i].revents & (POLLHUP | POLLERR))
			this->closeClientConnection(i);
	}
}

void	Manager::addToPollFds(int clientFd)
{
	pollfd newPollFd;
	newPollFd.fd = clientFd;
	newPollFd.events = POLLIN;
	this->_pollFdsVector.push_back(newPollFd);
}

void	Manager::delFromPollFds(int i)
{
	this->_pollFdsVector.erase(this->_pollFdsVector.begin() + i);
}

const char*	Manager::ServerSocketException::what(void) const throw()
{
	return ("Server socket: ");
}

const char*	Manager::ClientSocketException::what(void) const throw()
{
	return ("Client socket: ");
}

std::vector<pollfd>	Manager::getFdsVector(void)
{
	return this->_pollFdsVector;
}