/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MonitorSockets.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 14:48:56 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/24 14:55:12 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

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
				std::cout << RED << "Client " << clientFd << " 408 Request Timeout"  << RESET << std::endl;
				closeClientConnection(i);
				i--; // Adjust index after erasing the element
			}
		}
	}
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
	_fdMap[clientFd] = listenFd;
	std::cout << std::endl;
	printTimestamp();
	std::cout << YELLOW << "Server" << RESET << " accepted new connection on client socket " << clientFd << std::endl;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return clientFd;
}

void	Manager::closeClientConnection(unsigned long i)
{
	printTimestamp();
	std::cout << RED << "Closing " << RESET << "client socket " << RESET << _pollFdsVector[i].fd << std::endl;
	close(_pollFdsVector[i].fd);
	delFromPollFds(i);
}
