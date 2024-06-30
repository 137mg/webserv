/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/25 15:34:24 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/30 02:28:08 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "Server.hpp"

CGI::CGI(const Server& server, Manager &Manager) : _server(server), _Manager(Manager)
{
	this->_envp = nullptr;
	this->_status = 0;
}

CGI::~CGI(void)
{
	if (this->_envp)
	{
		for (int i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
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
	
	this->_envpVector.push_back(fullContent);
	this->_envpVector.push_back(fullContentLen);
	this->_envpVector.push_back(fullAddress);
	this->_envpVector.push_back(fullMethod);
	this->_envpVector.push_back(directoryPathEnv);
	this->_envpVector.push_back(fullPort);
	this->_envpVector.push_back(fullProtocol);
	this->_envpVector.push_back("");
}

void	CGI::convertVector(void)
{
	// Allocate memory for the char ** array
	this->_envp = new char*[this->_envpVector.size() + 1]; // +1 for the NULL terminator

	// Copy each string from the vector to the char ** array
	for (size_t i = 0; i < this->_envpVector.size(); ++i) {
		this->_envp[i] = new char[this->_envpVector[i].size() + 1];
		std::strcpy(this->_envp[i], this->_envpVector[i].c_str());
	}
	this->_envp[this->_envpVector.size()] = nullptr; // NULL terminator
}

Server & CGI::getServer(void)
{
    return _server;
}

void	CGI::errorHandler(int errorCode, int clientFd)
{
	this->_Manager.setClientStatus(clientFd, WRITING);
	this->getServer().buildErrorResponse(errorCode);
}

int	CGI::getStatus(void)
{
	return _status;
}
