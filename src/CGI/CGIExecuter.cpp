/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGIExecuter.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:12:47 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/27 16:52:57 by juvan-to      ########   odam.nl         */
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
	if (setUpFork() != 0)
		return errorHandler(500, _clientFd);
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
        return errorHandler(500, _clientFd);
    }
	else
	{
        close(_stdoutPipe[1]); // close write end of stdout pipe
        close(_stdinPipe[0]);  // close read end of stdin pipe

        t_CGIProcess cgi = {_stdinPipe[1], _stdoutPipe[0], clientFd, 0, "", "", _pid};
        cgi.cgiRequest = cgiRequest; // Store the request body
        cgi.cgiRequestSent = 0; // Track how much of the request body has been sent
		cgi.stdinFd = _stdinPipe[1];
		cgi.stdoutFd = _stdoutPipe[0];
		this->_Manager.setClientStatus(cgi.stdinFd, WRITING);
		this->_Manager.setClientStatus(cgi.stdoutFd, READING);
        _Manager.addCGIProcess(cgi); // Add CGI process to Manager
		_Manager.addToPollFds(cgi.stdinFd); // Add stdin pipe to poll list with POLLOUT
		_Manager.markFdForWriting(cgi.stdinFd);
		_Manager.addToPollFds(cgi.stdoutFd);

    }
}

        // if (WIFEXITED(_status))
        // {
        //     int exit_status = WEXITSTATUS(_status);
        //     if (exit_status != 0)
        //     {
        //         // Handle non-zero exit status
        //         std::cout << "error found" << std::endl;
        //         // return errorHandler(500, _clientFd);
        //     }
        // }