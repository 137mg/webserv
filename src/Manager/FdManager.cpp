/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FdManager.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:10:57 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/02 01:26:24 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::markFdForWriting(int clientFd)
{
    for (auto& pollFd : _pollFdsVector)
    {
        if (pollFd.fd == clientFd)
        {
            // Add POLLOUT flag to revents
            pollFd.events |= POLLOUT;
            return;
        }
    }
    // Handle case where clientFd is not found in _pollFdsVector
    throw std::runtime_error("Client fd not found in _pollFdsVector");
}

void	Manager::clearFdForWriting(int clientFd)
{
    for (auto& pollFd : _pollFdsVector)
    {
        if (pollFd.fd == clientFd)
        {
            // Remove POLLOUT flag from events
            pollFd.events &= ~POLLOUT;
            return;
        }
    }
    // Handle case where clientFd is not found in _pollFdsVector
    throw std::runtime_error("Client fd not found in _pollFdsVector");
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
