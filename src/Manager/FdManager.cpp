/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FdManager.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:10:57 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 01:52:36 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::markFdForWriting(int clientFd)
{
    for (size_t i = 0; i < _pollFdsVector.size(); i++)
    {
        if (_pollFdsVector[i].fd == clientFd)
        {
            _pollFdsVector[i].events = POLLOUT;
            _pollFdsVector[i].revents = 0;
            return;
        }
    }
}

void	Manager::markFdForReading(int clientFd)
{
    for (size_t i = 0; i < _pollFdsVector.size(); i++)
    {
        if (_pollFdsVector[i].fd == clientFd)
        {
            _pollFdsVector[i].events = POLLIN;
            _pollFdsVector[i].revents = 0;
            return;
        }
    }
}

void    Manager::addToPollFds(int clientFd, short events)
{
	pollfd newPollFd;
	newPollFd.fd = clientFd;
	newPollFd.events = events;
    newPollFd.revents = 0;
	_pollFdsVector.push_back(newPollFd);
}

void    Manager::setClientStatus(int fd, int status)
{
    this->_clientStatus[fd] = status;
}

std::map<int, int>	Manager::getClientStatus(void)
{
    return this->_clientStatus;
}
