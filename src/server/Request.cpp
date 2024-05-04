/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/04 15:39:05 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// handle the request received from the client
void	Server::handleRequest(std::string buffer, int bytesRead)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					requestedPath = parseRequest(buffer);

	terminalMessage("Client request ", buffer);
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	if (requestedPath.find("cgi-bin/upload.py") != std::string::npos)
	{
		postRequest(buffer, bytesRead);
		return ;
	}
	if (tokens[0].compare("GET") == 0)
		this->getRequest(tokens[1]);
	return;
}

// This is a very rough parsing function for now
std::string	Server::parseRequest(const std::string &request)
{
	std::string	path = "";
	size_t		start_pos = request.find(" ") + 1; // Find the position of the first space after the request method (e.g., "GET")
	
	if (start_pos != std::string::npos)
	{
		size_t end_pos = request.find(" ", start_pos); // Find the position of the next space after the start position
		if (start_pos != std::string::npos)
			path = request.substr(start_pos, end_pos - start_pos);
	}
	return path;
}
	
void	Server::getRequest(std::string file)
{
	std::string filePath = "";
	std::string response = "";

	if (file.compare("/") == 0)
		file = "/home.html";
	if (file.find(".html") != std::string::npos)
		filePath = "html" + file;
	else
		filePath = file.erase(0, 1);
	if (fileAccess(filePath))
		response = serveFile(filePath, "200 OK", GREEN);
	else
		response = serveFile("html/PageNotFound.html", "404 Not Found", RED);

	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("Server response ", response);
}

std::string	Server::serveFile(const std::string &path, const std::string &status, const std::string &color)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	responseStream << fileStream.rdbuf();
	
    std::string fileContents = responseStream.str();
	std::string response = "HTTP/1.1 " + color + status + RESET + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
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
	std::size_t	found = s1.find("Client");

	position = std::string(s2).find('\n');
	header = std::string(s2).substr(0, position);
	
	printTimestamp();
	if (found != std::string::npos)
		std::cout << RESET << BLUE << s1 << RESET << "from socket " << this->_clientFd << "	" << header << std::endl;
	else
		std::cout << RESET << PINK << s1 << RESET << "to socket " << this->_clientFd << "	" << header << std::endl;
}

std::string	Server::getHeader(std::string buffer, std::string key)
{
	size_t keyPos = buffer.find(key + ":");
    if (keyPos == std::string::npos) {
        // Key not found in the buffer
        return "";
    }

    // Find the end of the line containing the key
    size_t endOfLinePos = buffer.find("\r\n", keyPos);
    if (endOfLinePos == std::string::npos) {
        // End of line not found
        return "";
    }

    // Extract the value after the key
    size_t valueStartPos = keyPos + key.length() + 2; // Skip ": " after the key
    std::string value = buffer.substr(valueStartPos, endOfLinePos - valueStartPos);

    return value;
}
