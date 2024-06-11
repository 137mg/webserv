/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 14:53:32 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/06 14:10:25 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(const Server& server) : _server(server)
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

void	CGI::initEnvp(const t_header& header)
{
	std::string	fullContent = "CONTENT_TYPE=" + header.contentType;
	std::string	fullContentLen = "CONTENT_LENGTH=" + header.contentLength;
	std::string	fullAddress = "REMOTE_ADDR=" + _server.host;
	std::string	fullMethod = "REQUEST_METHOD=" + header.method;
	std::string	fullPort = "SERVER_PORT=" + std::to_string(_server.port);
	std::string	fullProtocol = "SERVER_PROTOCOL=" + header.protocol;
	
	this->_envpVector.push_back(fullContent);
	this->_envpVector.push_back(fullContentLen);
	this->_envpVector.push_back("HTTP_COOKIE=test");
	this->_envpVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_envpVector.push_back("HTTP_USER_AGENT=Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.1234.56 Safari/537.36");
	this->_envpVector.push_back("PATH_INFO=test");
	this->_envpVector.push_back("QUERY_STRING=");
	this->_envpVector.push_back(fullAddress);
	this->_envpVector.push_back("REMOTE_HOST=localhost");
	this->_envpVector.push_back(fullMethod);
	// this->_envpVector.push_back("SCRIPT_FILENAME=/cgi-bin/hello.py");
	// this->_envpVector.push_back("SCRIPT_NAME=hello.py");
	this->_envpVector.push_back("SERVER_NAME=Webserv");
	this->_envpVector.push_back(fullPort);
	this->_envpVector.push_back(fullProtocol);
	this->_envpVector.push_back("SERVER_SOFTWARE=webserv");
	this->_envpVector.push_back("REDIRECT_STATUS=200");
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


void	CGI::executeScript(std::string file, std::string cgiContent)
{
	int 		fds[2];
    pid_t 		pid;

    pipe(fds);
    pid = fork();
    if (pid == 0)
    {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);

        const char *args[] = {file.c_str(), nullptr};
        execve(file.c_str(), const_cast<char **>(args), this->_envp);
		perror("execve failed"); 
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fds[0]);
        write(fds[1], cgiContent.c_str(), cgiContent.size());
        close(fds[1]);
        waitpid(pid, nullptr, 0);
    }
}
