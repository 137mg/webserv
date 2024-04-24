/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Post.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: Julia <Julia@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/23 23:05:36 by Julia         #+#    #+#                 */
/*   Updated: 2024/04/25 00:17:03 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <sys/types.h>
#include <sys/wait.h>

void	Server::postRequest(std::string path, char *buffer)
{
	int		fds[2];
	pid_t	pid;

	pipe(fds);
	// std::cout << buffer << std::endl;
	pid = fork();
	if (pid == 0)
	{
		close(fds[1]);

		const char *contentType = "CONTENT_TYPE: multipart/form-data; boundary=----WebKitFormBoundaryBIeRFsXZV9QqAn3y";
		const char *contentLength = "CONTENT_LENGTH: 201";
		char *const envp[] = {const_cast<char*>(contentType), const_cast<char*>(contentLength), nullptr};
		
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		
		const char *args[] = {"./cgi-bin/upload.py", nullptr};
		
    	execve("./cgi-bin/upload.py", const_cast<char**>(args), envp);
		exit(EXIT_FAILURE);
	}
	close(fds[0]);
	close(fds[1]);
	waitpid(pid, nullptr, 0);
	std::string response = "";
	response = serveFile("html/home.html", "200 OK", GREEN);
	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("Server response to ", response);
	return;
	std::cout << path + buffer << std::endl;
}
