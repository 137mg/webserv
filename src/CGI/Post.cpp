/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Post.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: Julia <Julia@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/23 23:05:36 by Julia         #+#    #+#                 */
/*   Updated: 2024/04/29 13:48:54 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"
#include <sys/types.h>
#include <sys/wait.h>

void	Server::postRequest(std::string path, char *buffer, int bytesRead)
{
	int fds[2];
    pid_t pid;
    CGI cgi;

    std::cout << buffer << std::endl;
    exit(1);
    cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"));
    cgi.convertVector();
    pipe(fds); // Create a pipe
    pid = fork(); // Fork the current process
    if (pid == 0)
    {
        // Child process
        close(fds[1]); // Close the write-end of the pipe in the child process
        dup2(fds[0], STDIN_FILENO); // Redirect standard input to the read-end of the pipe
        close(fds[0]); // Close the original read-end of the pipe

        const char *args[] = {"./cgi-bin/upload.py", nullptr};
        execve("./cgi-bin/upload.py", const_cast<char **>(args), cgi.getEnvp());
        exit(EXIT_FAILURE); // Exit if execve fails
    }
    else
    {
        // Parent process
        close(fds[0]); // Close the read-end of the pipe in the parent process
        // Write the received data to the write-end of the pipe
        write(fds[1], buffer, bytesRead);
        close(fds[1]); // Close the write-end of the pipe after writing

        waitpid(pid, nullptr, 0); // Wait for the child process to finish

        // Prepare and send the response back to the client
        std::string response = "";
        response = serveFile("html/home.html", "200 OK", GREEN);
        write(this->_clientFd, response.c_str(), response.size());
        terminalMessage("Server response to ", response);
    }
    // Debugging prints (optional)
	return;
    std::cout << bytesRead << std::endl;
    std::cout << path + buffer << std::endl;
}
