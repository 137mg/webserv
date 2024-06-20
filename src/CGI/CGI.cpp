/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:53:32 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/20 15:26:03 by juvan-to      ########   odam.nl         */
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

void	CGI::initEnvp(const t_header& header)
{
	std::string	fullContent = "CONTENT_TYPE=" + header.contentType;
	std::string	fullContentLen = "CONTENT_LENGTH=" + header.contentLength;
	std::string	fullAddress = "REMOTE_ADDR=" + _server.host;
	std::string	fullMethod = "REQUEST_METHOD=" + header.method;
	std::string	fullPort = "SERVER_PORT=" + std::to_string(_server.port);
	std::string	fullProtocol = "SERVER_PROTOCOL=" + header.protocol;
	std::string directoryPathEnv = "INPUT=" + header.file;
	
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
	this->_envpVector.push_back(directoryPathEnv);
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

void	CGI::executeScript(std::string file, std::string cgiRequest, int clientFd)
{
	int 	stdoutPipe[2];
    int		stdinPipe[2];
    pid_t	pid;

    if (pipe(stdoutPipe) == -1 || pipe(stdinPipe) == -1)
	{
        perror("pipe failed");
    }

    pid = fork();
    if (pid == -1)
	{
        perror("fork failed");
    }

    if (pid == 0)
	{
        close(stdoutPipe[0]); // close read end of stdout pipe
        close(stdinPipe[1]);  // close write end of stdin pipe

        dup2(stdinPipe[0], STDIN_FILENO); // redirect stdin
        dup2(stdoutPipe[1], STDOUT_FILENO); // redirect stdout

        close(stdinPipe[0]);
        close(stdoutPipe[1]);

        const char *args[] = {file.c_str(), nullptr};
        execve(file.c_str(), const_cast<char **>(args), _envp);
        perror("execve failed");
        exit(EXIT_FAILURE);
    }
	else
	{
        close(stdoutPipe[1]); // close write end of stdout pipe
        close(stdinPipe[0]);  // close read end of stdin pipe


        t_CGIProcess cgi = {stdinPipe[1], stdoutPipe[0], clientFd, 0, "", "", pid};
        cgi.cgiRequest = cgiRequest; // Store the request body
        cgi.cgiRequestSent = 0; // Track how much of the request body has been sent
		cgi.stdinFd = stdinPipe[1];
		cgi.stdoutFd = stdoutPipe[0];
		
        _Manager.addCGIProcess(cgi); // Add CGI process to Manager
		_Manager.addToPollFds(stdinPipe[1]); // Add stdin pipe to poll list with POLLOUT
		_Manager.markFdForWriting(stdinPipe[1]);
		_Manager.addToPollFds(stdoutPipe[0]);
    }
	return;
}

    // write(stdinPipe[1], cgiRequest.c_str(), cgiRequest.size());