/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIFdManager.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:06:44 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/02 01:54:54 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "CGI.hpp"
#include "Server.hpp"

bool	Manager::isCGIInputFd(int fd)
{
    for (size_t i = 0; i < _cgiProcesses.size(); i++)
    {
        if (_cgiProcesses[i].stdinFd == fd)
            return true;
    }
    return false;
}

void Manager::removeCGIProcess(int fd)
{
    for (size_t i = 0; i < _cgiProcesses.size();)
    {
        if (_cgiProcesses[i].stdinFd == fd || _cgiProcesses[i].stdoutFd == fd || _cgiProcesses[i].clientFd == fd)
        {
            _cgiProcesses.erase(_cgiProcesses.begin() + i);
            return;
        }
        else
            ++i;
    }
}

t_CGIProcess& Manager::getCGIProcessForFd(int fd)
{
    for (size_t i = 0; i < _cgiProcesses.size(); i++)
    {
        if (_cgiProcesses[i].stdinFd == fd || _cgiProcesses[i].stdoutFd == fd || _cgiProcesses[i].clientFd == fd)
            return _cgiProcesses[i];
    }
    throw std::runtime_error("CGI process not found for given file descriptor");
}

void Manager::delFromPollFdsByValue(int fd)
{
    for (size_t i = 0; i < _pollFdsVector.size(); i++)
    {
        if (_pollFdsVector[i].fd == fd)
        {
            _pollFdsVector.erase(_pollFdsVector.begin() + i);
            return;
        }
    }
}

bool	Manager::checkIfCGIProcessExistsForFd(int fd)
{
    for (size_t i = 0; i < _cgiProcesses.size(); i++)
    {
        if (_cgiProcesses[i].stdoutFd == fd)
            return true;
    }
    return false;
}

void    Manager::addCGIProcess(t_CGIProcess cgiProcess)
{
	_cgiProcesses.push_back(cgiProcess);
}

void    CGI::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL");
        exit(EXIT_FAILURE);
    }
}