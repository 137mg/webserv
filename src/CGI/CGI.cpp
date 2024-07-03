/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:34:24 by juvan-to          #+#    #+#             */
/*   Updated: 2024/07/03 14:51:35 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(const Server& server, Manager &Manager) : _server(server), _Manager(Manager)
{
	_envp = nullptr;
	_status = 0;
}

CGI::~CGI(void)
{
	if (_envp)
	{
		for (int i = 0; _envp[i]; i++)
			delete[] _envp[i];
		delete[] _envp;
	}
}

void	CGI::initEnvp(const t_header& header, std::string request)
{
	std::string	fullContent = "CONTENT_TYPE=" + header.contentType;
	std::string	fullContentLen = "CONTENT_LENGTH=" + std::to_string(request.size());
	std::string	fullAddress = "REMOTE_ADDR=" + _server.host;
	std::string	fullMethod = "REQUEST_METHOD=" + header.method;
	std::string	fullPort = "SERVER_PORT=" + std::to_string(_server.port);
	std::string	fullProtocol = "SERVER_PROTOCOL=" + header.protocol;
	std::string directoryPathEnv = "INPUT=" + header.file;
	
	_envpVector.push_back(fullContent);
	_envpVector.push_back(fullContentLen);
	_envpVector.push_back(fullAddress);
	_envpVector.push_back(fullMethod);
	_envpVector.push_back(directoryPathEnv);
	_envpVector.push_back(fullPort);
	_envpVector.push_back(fullProtocol);
	_envpVector.push_back("");
}

void	CGI::convertVector(void)
{
	// Allocate memory for the char ** array
	_envp = new char*[_envpVector.size() + 1]; // +1 for the NULL terminator

	// Copy each string from the vector to the char ** array
	for (size_t i = 0; i < _envpVector.size(); ++i) {
		_envp[i] = new char[_envpVector[i].size() + 1];
		std::strcpy(_envp[i], _envpVector[i].c_str());
	}
	_envp[_envpVector.size()] = nullptr; // NULL terminator
}

void	CGI::errorHandler(int errorCode, int clientFd)
{
	_Manager.setClientStatus(clientFd, WRITING);
	_server.buildErrorResponse(errorCode);
}

void	CGI::setNonBlocking(int fd)
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
