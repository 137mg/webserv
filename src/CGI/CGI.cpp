/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:53:32 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/30 00:00:33 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(void)
{
	this->_envp = nullptr;
}

CGI::~CGI(void)
{
	
}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI & CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->_envp = other._envp;
	}
	return *this;
}

char	**CGI::getEnvp(void) const
{
	return this->_envp;
}

void	CGI::initEnvp(std::string content, std::string contentLength)
{
	std::string	fullContent = "CONTENT_TYPE=" + content;
	std::string	fullContentLen = "CONTENT_LENGTH=" + contentLength;
	
	this->_envpVector.push_back(fullContent);
	this->_envpVector.push_back(fullContentLen);
	this->_envpVector.push_back("HTTP_COOKIE=test");
	this->_envpVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_envpVector.push_back("HTTP_USER_AGENT=Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.1234.56 Safari/537.36");
	this->_envpVector.push_back("PATH_INFO=test");
	this->_envpVector.push_back("QUERY_STRING=");
	this->_envpVector.push_back("REMOTE_ADDR=127.0.0.1");
	this->_envpVector.push_back("REMOTE_HOST=localhost");
	this->_envpVector.push_back("REQUEST_METHOD=POST");
	this->_envpVector.push_back("SCRIPT_FILENAME=/cgi-bin/upload.py");
	this->_envpVector.push_back("SCRIPT_NAME=upload.py");
	this->_envpVector.push_back("SERVER_NAME=Webserv");
	this->_envpVector.push_back("SERVER_PORT=8080");
	this->_envpVector.push_back("SERVER_PROTOCOL=HTTP/1.1");
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

void	CGI::executeScript(std::string cgiContent, int bytesRead)
{
	int 		fds[2];
    pid_t 		pid;

    this->convertVector();
    pipe(fds);
    pid = fork();
    if (pid == 0)
    {
        close(fds[1]);
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);

        const char *args[] = {"./cgi-bin/upload.py", nullptr};
        execve("./cgi-bin/upload.py", const_cast<char **>(args), this->getEnvp());
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fds[0]);
        write(fds[1], cgiContent.c_str(), bytesRead);
        close(fds[1]);
        waitpid(pid, nullptr, 0);
    }
}
