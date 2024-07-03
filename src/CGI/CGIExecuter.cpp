/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIExecuter.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:12:47 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 17:07:12 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

int	CGI::setUpPipes(void)
{
	if (pipe(_stdoutPipe) < 0)
	{
	perror("Pipe error");
		return (-1);
	}
	if (pipe(_stdinPipe) < 0)
	{
		close(_stdoutPipe[0]);
		close(_stdoutPipe[1]);
		perror("Pipe error");
		return (-1);
	}
	return (0);
}

int	CGI::setUpFork(void)
{
	_pid = fork();
	if (_pid < 0)
	{
		close(_stdoutPipe[0]);
		close(_stdoutPipe[1]);
		close(_stdinPipe[0]);
		close(_stdinPipe[1]);
		perror("Fork error");
		return (-1);
	}
	return (0);
}

void	CGI::executeScript(std::string scriptPath, std::string CGIdirectory, std::string cgiRequest, int clientFd)
{
	_clientFd = clientFd;
	if (setUpPipes() != 0)
		return errorHandler(500, _clientFd);
    setNonBlocking(_stdinPipe[1]);
    setNonBlocking(_stdoutPipe[0]);
    setNonBlocking(_stdinPipe[0]);
    setNonBlocking(_stdoutPipe[1]);
	if (setUpFork() != 0)
		return errorHandler(500, _clientFd);
    
    t_CGIProcess cgi = {_stdinPipe[1], _stdoutPipe[0], clientFd, 0, 0,0, 0, "", "", "", "", _pid};
    if (_pid == 0)
	{
        close(_stdoutPipe[0]);
        close(_stdinPipe[1]);

        dup2(_stdinPipe[0], STDIN_FILENO);
        dup2(_stdoutPipe[1], STDOUT_FILENO);

		close(_stdinPipe[0]);
		close(_stdoutPipe[1]);

        if (chdir(CGIdirectory.c_str()) != 0)
        {
            perror("Chdir error");
            exit(EXIT_FAILURE);
        }
        const char *args[] = {"python3", scriptPath.c_str(), nullptr};
        execve(scriptPath.c_str(), const_cast<char **>(args), _envp);
        perror("Execve error");
        exit(EXIT_FAILURE);
    }
	else
	{
        close(_stdoutPipe[1]);
        close(_stdinPipe[0]);

        cgi.cgiRequest = cgiRequest;
        cgi.cgiRequestWritten = 0;
        cgi.stdinFd = _stdinPipe[1];
		cgi.stdoutFd = _stdoutPipe[0];
        cgi.status = 0;

        _Manager.addCGIProcess(cgi);
		_Manager.addToPollFds(cgi.stdinFd, POLLOUT);
		_Manager.addToPollFds(cgi.stdoutFd, POLLIN);

		_Manager.markFdForWriting(cgi.stdinFd);
		_Manager.setClientStatus(cgi.stdinFd, WRITING);
		_Manager.setClientStatus(cgi.stdoutFd, READING);
    }
}
