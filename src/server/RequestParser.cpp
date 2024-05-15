/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/15 14:46:58 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// handle the request received from the client
void	Server::handleRequest(std::string buffer)
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
		postRequest(buffer);
		return ;
	}
	if (tokens[0].compare("GET") == 0)
		this->getRequest(tokens[1]);
	else if (tokens[0].compare("DELETE") == 0)
		this->deleteRequest(tokens[1]);
	return;
}

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

// Returns the value of a specific header
std::string	Server::getHeader(std::string buffer, std::string key)
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
