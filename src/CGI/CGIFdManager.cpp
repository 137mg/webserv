/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIFdManager.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:06:44 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/02 01:42:23 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "CGI.hpp"
#include "Server.hpp"

bool	Manager::isCGIInputFd(int fd)
{
    for (const auto& cgi : _cgiProcesses)
    {
        if (cgi.stdinFd == fd)
        {
            return true;
        }
    }
    return false;
}

void Manager::removeCGIProcess(int fd)
{
	auto it = std::remove_if(_cgiProcesses.begin(), _cgiProcesses.end(),
							 [fd](const t_CGIProcess &cgi) {
								 return cgi.stdinFd == fd || cgi.stdoutFd == fd || cgi.clientFd == fd;
							 });
	if (it != _cgiProcesses.end())
	{
		_cgiProcesses.erase(it, _cgiProcesses.end());
	}
}

t_CGIProcess& Manager::getCGIProcessForFd(int fd)
{
    for (auto& cgi : _cgiProcesses)
    {
        if (cgi.stdinFd == fd || cgi.stdoutFd == fd || cgi.clientFd == fd)
        {
            return cgi;
        }
    }
	throw std::runtime_error("CGI process not found for given file descriptor");
}

void Manager::delFromPollFdsByValue(int fd)
{
    auto it = std::find_if(this->_pollFdsVector.begin(), this->_pollFdsVector.end(),
                           [fd](const pollfd& pfd) { return pfd.fd == fd; });
    if (it != this->_pollFdsVector.end())
    {
        this->_pollFdsVector.erase(it);
    }
}

bool	Manager::checkIfCGIProcessExistsForFd(int fd)
{
	// Find the CGI process in _cgiProcesses whose stdoutFd matches fd
	std::vector<t_CGIProcess>::iterator it = std::find_if(_cgiProcesses.begin()
								,_cgiProcesses.end(), [&](const t_CGIProcess &cgi)
		{
			return cgi.stdoutFd == fd;
		});

	// Check if the iterator points to a valid element in _cgiProcesses
	bool found = (it != _cgiProcesses.end());

	// Return true if the CGI process with stdoutFd equal to fd was found, otherwise false
	return found;
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