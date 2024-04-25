/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:53:32 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/25 18:00:07 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(void)
{
	this->_envp = nullptr;
	initEnvp();
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

void	CGI::initEnvp(void)
{
	this->_envpVector.push_back("CONTENT_TYPE=");
	this->_envpVector.push_back("CONTENT_LENGTH=");
	this->_envpVector.push_back("HTTP_COOKIE=");
	this->_envpVector.push_back("HTTP_USER_AGENT=");
	this->_envpVector.push_back("PATH_INFO=");
	this->_envpVector.push_back("QUERY_STRING=");
	this->_envpVector.push_back("REMOTE_ADDR=");
	this->_envpVector.push_back("REMOTE_HOST=");
	this->_envpVector.push_back("REQUEST_METHOD=");
	this->_envpVector.push_back("SCRIPT_FILENAME=");
	this->_envpVector.push_back("SCRIPT_NAME=");
	this->_envpVector.push_back("SERVER_NAME=");
	this->_envpVector.push_back("SERVER_SOFTWARE=");
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
	std::string	response = "";
	pid_t		pid;

	convertVector();
	pid = fork();
	if (pid == 0)
	{
		const char *args[] = {"./cgi-bin/upload.py", nullptr};
    	execve("./cgi-bin/upload.py", const_cast<char**>(args), this->_envp);
	}
	else
	{		
		waitpid(pid, nullptr, 0);
	}
	return response;
}


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