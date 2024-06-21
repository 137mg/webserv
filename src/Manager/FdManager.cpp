/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   FdManager.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:10:57 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/21 13:48:41 by juvan-to      ########   odam.nl         */
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

std::vector<pollfd> Manager::getFdsVector(void)
{
	return this->_pollFdsVector;
}
