/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/15 15:03:47 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// to be implemented, will handle the GET and POST requests
void	Server::handleRequest(char *buffer)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					method;
	std::string					file;

	std::cout << "Handling request..." << std::endl;
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	
	method = tokens[0];
	file = tokens[1];
	std::cout << method << std::endl;
	std::cout << file << std::endl;
	if (method.compare("GET") == 0)
		this->getPage(file);
	return;	
}

void	Server::getPage(std::string file)
{
	std::string filePath;

	filePath = "html" + file;
	std::ifstream fileStream(filePath);
	if (!fileStream)
	{
		// Send a 404 Not Found response
		std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found: The requested resource could not be found";
		send(this->_clientFd, response.c_str(), response.size(), 0);
		close(this->_clientFd);
	}
	else
	{
		// Read the file contents into a string
        std::stringstream responseStream;
		responseStream << fileStream.rdbuf();
        std::string fileContents = responseStream.str();
        // Send the file contents as the HTTP response
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(fileContents.size()) + "\r\nContent-Type: text/html\r\n\r\n" + fileContents;
        send(this->_clientFd, response.c_str(), response.size(), 0);	
		close(this->_clientFd);
	}
}
