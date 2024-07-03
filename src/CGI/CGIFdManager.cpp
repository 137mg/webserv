/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIFdManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 13:06:44 by juvan-to          #+#    #+#             */
/*   Updated: 2024/07/03 14:57:33 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

bool	Manager::isCGIInputFd(int fd)
{
	for (size_t i = 0; i < _cgiProcesses.size(); i++)
	{
		if (_cgiProcesses[i].stdinFd == fd)
			return (true);
	}
	return (false);
}

bool	Manager::isCGIOutputFd(int fd)
{
	for (size_t i = 0; i < _cgiProcesses.size(); i++)
	{
		if (_cgiProcesses[i].stdoutFd == fd)
			return (true);
	}
	return (false);
}

void	Manager::removeCGIProcess(int fd)
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

t_CGIProcess&	Manager::getCGIProcessForFd(int fd)
{
	for (size_t i = 0; i < _cgiProcesses.size(); i++)
	{
		if (_cgiProcesses[i].stdinFd == fd || _cgiProcesses[i].stdoutFd == fd || _cgiProcesses[i].clientFd == fd)
			return (_cgiProcesses[i]);
	}
	throw std::runtime_error("CGI process not found for given file descriptor");
}

void	Manager::addCGIProcess(t_CGIProcess cgiProcess)
{
	_cgiProcesses.push_back(cgiProcess);
}
