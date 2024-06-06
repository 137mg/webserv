/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethods.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/06 12:21:26 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

std::string executeCgiScript(const std::string& scriptPath)
{
    // Create pipes for inter-process communication
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if (pid == 0) { // Child process
        // Close the read end of the pipe
        close(fd[0]);
        
        // Redirect stdout to the write end of the pipe
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        
        // Execute the CGI script
        execlp("python3", "python3", scriptPath.c_str(), NULL);
        exit(1); // If execlp fails
    } else { // Parent process
        // Close the write end of the pipe
        close(fd[1]);
        
        // Read the output from the read end of the pipe
        char buffer[1024];
        std::string result;
        ssize_t bytesRead;
        while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }
        close(fd[0]);
        
        // Wait for the child process to finish
        waitpid(pid, NULL, 0);
        
        // Return the CGI script output as the response
        return result;
    }
}

void	Server::getMethod(void)
{
	std::string	filePath;
	std::string	response;

	if (_header.file == "/")
		_header.file += _location.index;
	// Determine the file path based on the file extension
	if (_header.file.find(".html") != std::string::npos)
		filePath = "html" + _header.file;
	else
		filePath = _header.file.erase(0, 1); // erase the leading slash so /index.html becomes index.html
	if (filePath == "html/files.html")
		response = showUploads(filePath, "200 OK", GREEN);
	else
	{
		if (access(filePath.c_str(), R_OK) == 0)
		{
			if (_header.file.find(".py") != std::string::npos)
			{
				runCGI(filePath, _request);
				return;
			}
			else
				response = serveFile(filePath, "200 OK");
		}
		else
			response = serveFile("html/error_pages/PageNotFound.html", "404 Not Found");		
	}
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(void)
{
	std::string	fullFilePath;
	std::string	response;

	fullFilePath = "cgi-bin/uploads" + _header.file;
	// need to decide what to do with delete error!
	if (std::remove(fullFilePath.c_str()) != 0)
		perror("Error deleting file");
	response = serveFile("html/files.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::postMethod(void)
{
	CGI			cgi;
	std::string	response;

	cgi.initEnvp(_header.contentType, _header.contentLength, _header.method);
	cgi.convertVector();
	cgi.executeScript("./cgi-bin/upload.py", _request);
	response = serveFile("html/home.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::runCGI(std::string filepath, std::string buffer)
{
	CGI	cgi;
	std::string response;

	cgi.initEnvp(_header.contentType, _header.contentLength, _header.method);
	cgi.convertVector();
	filepath = "./" + filepath;
	if (access(filepath.c_str(), X_OK) == 0)
	{
		cgi.executeScript(filepath, buffer);
		response = serveFile("html/home.html", "200 OK");
		write(_clientFd, response.c_str(), response.size());
		serverMessage(response, _clientFd, GREEN);
	}
	else
	{
		response = serveFile("html/error_pages/Forbidden.html", "403 Forbidden");
		write(_clientFd, response.c_str(), response.size());
		serverMessage(response, _clientFd, GREEN);
	}
}
