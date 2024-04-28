/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:53:32 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/29 00:45:49 by Julia         ########   odam.nl         */
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

std::string	CGI::executeScript(void)
{
	int		fds[2];
	pid_t	pid;

	convertVector();
	pipe(fds);
	pid = fork();
	if (pid == 0)
	{
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		close(fds[1]);

		const char *args[] = {"./cgi-bin/upload.py", nullptr};
    	execve("./cgi-bin/upload.py", const_cast<char**>(args), this->_envp);
		exit(EXIT_FAILURE);
	}
	close(fds[0]);
	close(fds[1]);
	waitpid(pid, nullptr, 0);
	std::string response = "";
	response = serveFile("html/home.html", "200 OK", GREEN);
	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("Server response to ", response);
	return response;
}

// std::string	CGI::executeScript(void)
// {
// 	std::string	response = "";
// 	pid_t		pid;

// 	convertVector();
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		const char *args[] = {"./cgi-bin/upload.py", nullptr};
//     	execve("./cgi-bin/upload.py", const_cast<char**>(args), this->_envp);
// 	}
// 	else
// 	{		
// 		waitpid(pid, nullptr, 0);
// 	}
// 	return response;
// }

// std::string	CGI::executeScript1(void)
// {
// 	std::string	response = "";
// 	int			fdsRequest[2];
// 	int			fdsResponse[2];
// 	pid_t		pid;

// 	convertVector();
// 	pipe(fdsRequest);
// 	pipe(fdsResponse);
// 	pid = fork();
// 	if (pid == 0)
// 	{
// 		dup2(fdsResponse[1], STDOUT_FILENO);
// 		close(fdsResponse[0]);
// 		close(fdsResponse[1]);
		
// 		dup2(fdsRequest[0], STDIN_FILENO);
// 		close(fdsRequest[1]);
// 		close(fdsRequest[0]);

// 		const char *args[] = {"./cgi-bin/upload.py", nullptr};
//     	execve("./cgi-bin/upload.py", const_cast<char**>(args), this->_envp);
// 		exit(EXIT_FAILURE);
// 	}
// 	else
// 	{
// 		close(fdsRequest[0]);
// 		close(fdsResponse[1]);
		
// 		write(fdsRequest[1], response.c_str(), response.size());
// 		close(fdsRequest[1]);
		
// 		waitpid(pid, nullptr, 0);
	
// 		char buffer_[1024];
// 		int ret = 0;
// 		do {
// 			std::memset(buffer_, 0, 1024);
// 			ret = read(fdsResponse[0], buffer_, 1024) ;
// 			response.append(buffer_, ret);

// 		} while (ret > 0);
// 		close(fdsResponse[0]);
// 	}
// 	return response;
// }