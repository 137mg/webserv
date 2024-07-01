/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIExecuter.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:12:47 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/02 01:56:41 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

int	CGI::setUpPipes(void)
{
	if (pipe(_stdoutPipe) < 0)
	{
        perror("Pipe error");
		return -1;
    }
	if (pipe(_stdinPipe) < 0)
	{
		close(_stdoutPipe[0]);
        close(_stdoutPipe[1]);
        perror("Pipe error");
		return -1;
    }
	return 0;
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
		return -1;
    }
	return 0;
}

void	CGI::executeScript(std::string CGIfile, std::string CGIdirectory, std::string cgiRequest, int clientFd)
{
    this->_clientFd = clientFd;
	if (setUpPipes() != 0)
		return errorHandler(500, _clientFd);
    setNonBlocking(_stdinPipe[1]);
    setNonBlocking(_stdoutPipe[0]);
	if (setUpFork() != 0)
		return errorHandler(500, _clientFd);
    t_CGIProcess cgi = {_stdinPipe[1], _stdoutPipe[0], clientFd, 0, 0,0, 0, "", "", "", "", _pid};

    if (_pid == 0)
	{
        close(_stdoutPipe[0]); // close read end of stdout pipe
        close(_stdinPipe[1]);  // close write end of stdin pipe

        dup2(_stdinPipe[0], STDIN_FILENO); // redirect stdin
        dup2(_stdoutPipe[1], STDOUT_FILENO); // redirect stdout

        close(_stdinPipe[0]);
        close(_stdoutPipe[1]);

		// Change the working directory to the script's directory
        if (chdir(CGIdirectory.c_str()) != 0)
        {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }
        const char *args[] = {CGIfile.c_str(), nullptr};
        execve(CGIfile.c_str(), const_cast<char **>(args), _envp);
        perror("execve failed");
        exit(EXIT_FAILURE);
    }
	else
	{
        close(_stdoutPipe[1]); // close write end of stdout pipe
        close(_stdinPipe[0]);  // close read end of stdin pipe

        cgi.cgiRequest = cgiRequest; // Store the request body
        cgi.cgiRequestWritten = 0; // Track how much of the request body has been sent
        cgi.stdinFd = _stdinPipe[1];
		cgi.stdoutFd = _stdoutPipe[0];
        cgi.cgiErrorResponse = errorResponse;
        cgi.status = 0;
        _Manager.addCGIProcess(cgi); // Add CGI process to Manager
		_Manager.addToPollFds(cgi.stdinFd, POLLOUT); // Add stdin pipe to poll list with POLLOUT
		_Manager.addToPollFds(cgi.stdoutFd, POLLIN);

		_Manager.markFdForWriting(cgi.stdinFd);
		this->_Manager.setClientStatus(cgi.stdinFd, WRITING);
		this->_Manager.setClientStatus(cgi.stdoutFd, READING);
    }
}
