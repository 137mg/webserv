/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   MonitorSockets.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/24 14:48:56 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/07/04 15:01:36 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

void	Manager::monitorSockets(void)
{
	while (RUNNING)
	{
		_status = poll(_pollFdsVector.data(), _pollFdsVector.size(), 2000);
		if (_status == -1)
			throw ServerSocketException();
		checkForTimeouts();
		if (_status != 0)
			handleSocketEvents();
	}
}

void	Manager::handleSocketEvents(void)
{
	for (size_t i = 0; i < _pollFdsVector.size() && RUNNING; i++)
	{
		if ((_pollFdsVector[i].revents & POLLIN) && _clientStatus[_pollFdsVector[i].fd] == READING)
		{
			if (isCGIOutputFd(_pollFdsVector[i].fd))
			{
				handleCGIOutput(_pollFdsVector[i].fd);
				continue;
			}

			if (std::find(_listenFds.begin(), _listenFds.end(), _pollFdsVector[i].fd) != _listenFds.end()) 
				newClientConnection(_pollFdsVector[i].fd);
			else
			{
				if (!readRequest(_pollFdsVector[i].fd))
				{
					closeClientConnection(_pollFdsVector[i].fd);
					break;
				}
				_clientActivityMap[_pollFdsVector[i].fd] = std::time(nullptr); // Update the last activity time
			}
		}
		else if ((_pollFdsVector[i].revents & POLLOUT) && _clientStatus[_pollFdsVector[i].fd] == WRITING)
		{
			if (isCGIInputFd(_pollFdsVector[i].fd))
			{
				handleCGIInput(_pollFdsVector[i].fd);
				continue;
			}
			sendPendingResponse(_pollFdsVector[i].fd);
		}
		else if (_pollFdsVector[i].revents & (POLLHUP | POLLERR))
		{
			if (isCGIOutputFd(_pollFdsVector[i].fd) && _pollFdsVector[i].revents & (POLLHUP))
			{
				handleHangup(_pollFdsVector[i].fd);
				continue;
			}
			closeClientConnection(_pollFdsVector[i].fd);
		}
	}
}

void	Manager::checkForTimeouts(void)
{
	time_t now = std::time(nullptr);
	for (size_t i = 0; i < _pollFdsVector.size(); i++)
	{
		int clientFd = _pollFdsVector[i].fd;
		if (_clientActivityMap.find(clientFd) != _clientActivityMap.end())
		{
			if (now - _clientActivityMap[clientFd] > _timeout)
			{
				printTimestamp();
				std::cout << RED << "Client " << clientFd << " 408 Request Timeout"  << RESET << std::endl;
				closeClientConnection(clientFd);
				i--;
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
	addToPollFds(clientFd, POLLIN);
	_clientActivityMap[clientFd] = std::time(nullptr);
	_fdMap[clientFd] = listenFd;
	_clientStatus[clientFd] = READING;
	_clientBuffers[clientFd] = "";
	std::cout << std::endl;
	printTimestamp();
	std::cout << YELLOW << "Server" << RESET << " accepted new connection on client socket " << clientFd << std::endl;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return (clientFd);
}

void	Manager::closeClientConnection(int clientFd)
{
	_clientActivityMap.erase(clientFd);
	_fdMap.erase(clientFd);
	_clientStatus.erase(clientFd);
	_clientBuffers.erase(clientFd);
	printTimestamp();
	std::cout << RED << "Closing " << RESET << "client socket " << RESET << clientFd << std::endl;
	close(clientFd);
	for (size_t i = 0; i < _pollFdsVector.size(); i++)
	{
		if (clientFd == _pollFdsVector[i].fd)
			_pollFdsVector.erase(_pollFdsVector.begin() + i);
	}
}
