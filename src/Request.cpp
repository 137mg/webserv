/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/18 17:44:34 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// handle the request received from the client (for now only GET requests)
void	Server::handleRequest(char *buffer)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);

	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	terminalMessage("Client request from ", buffer);

	if (tokens[0].compare("GET") == 0)
		this->getRequest(tokens[1]);
	return;	
}

void	Server::getRequest(std::string file)
{
	std::string filePath = "";
	std::string response = "";

	if (file.compare("/") == 0)
		file = "/index.html";
	filePath = "html" + file;

	if (fileAccess(filePath))
		response = serveFile(filePath, "200 OK", GREEN);
	else
		response = serveFile("html/PageNotFound.html", "404 Not Found", RED);

	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("Server response to ", response);
}

std::string	Server::serveFile(const std::string &path, const std::string &status, const std::string &color)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	responseStream << fileStream.rdbuf();
	
    std::string fileContents = responseStream.str();
	std::string response = "HTTP/1.1 " + color + status + RESET + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return response;
}


bool	Server::fileAccess(const std::string &path)
{
	std::ifstream fileStream(path);
	return fileStream.good();
}

void	Server::terminalMessage(const std::string &s1, const std::string &s2)
{
	size_t		position;
	std::string	header;

	position = std::string(s2).find('\n');
	header = std::string(s2).substr(0, position);
	
	printTimestamp();
	std::cout << RESET << s1 << "socket " << this->_clientFd << "	" << header << std::endl;
}