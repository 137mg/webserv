/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/21 16:11:25 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// handle the request received from the client
void	ServerManager::handleRequest(std::string buffer, int clientFd)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					requestedPath = parseRequest(buffer);

	terminalMessage("Client request ", buffer, clientFd);
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	if (requestedPath.find("cgi-bin/upload.py") != std::string::npos)
	{
		postRequest(buffer, tokens[0], clientFd);
		return ;
	}
	if (tokens[0].compare("GET") == 0)
		this->getRequest(tokens[1], clientFd);
	else if (tokens[0].compare("DELETE") == 0)
		this->deleteRequest(tokens[1], clientFd);
	return;
}

std::string	ServerManager::parseRequest(const std::string &request)
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

// Returns the value of a specific header
std::string	ServerManager::getHeader(std::string buffer, std::string key)
{
	size_t keyPos = buffer.find(key + ":");
	
    if (keyPos == std::string::npos)
	{
        // Key not found in the buffer
        return "";
    }

    // Find the end of the line containing the key
    size_t endOfLinePos = buffer.find("\r\n", keyPos);
    if (endOfLinePos == std::string::npos)
	{
        // End of line not found
        return "";
    }

    // Extract the value after the key
    size_t valueStartPos = keyPos + key.length() + 2; // Skip ": " after the key
    std::string value = buffer.substr(valueStartPos, endOfLinePos - valueStartPos);
    return value;
}