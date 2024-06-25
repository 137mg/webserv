/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/25 15:34:24 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/25 15:52:38 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(const Server& server, Manager &Manager) : _server(server), _Manager(Manager)
{
	this->_envp = nullptr;
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
	this->_envpVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_envpVector.push_back("HTTP_USER_AGENT=Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.1234.56 Safari/537.36");
	this->_envpVector.push_back("PATH_INFO=test");
	this->_envpVector.push_back("QUERY_STRING=");
	this->_envpVector.push_back(fullAddress);
	this->_envpVector.push_back("REMOTE_HOST=localhost");
	this->_envpVector.push_back(fullMethod);
	this->_envpVector.push_back(directoryPathEnv);
	// this->_envpVector.push_back("SCRIPT_FILENAME=/cgi-bin/hello.py");
	// this->_envpVector.push_back("SCRIPT_NAME=hello.py");
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
