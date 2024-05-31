/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 15:46:02 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// placeholder
Server&	ServerManager::chooseServer(void)
{
	return (_serverMap[_ports[0]][0]);
}

// changed a bit, need to change a lot more!
void	ServerManager::handleRequest(std::string buffer, int clientFd)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					requestedPath = parseRequest(buffer);

	terminalMessage("Client request ", buffer, clientFd);
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	
	Server server = chooseServer();

	if (requestedPath.find("cgi-bin/upload.py") != std::string::npos)
	{
		server.postRequest(buffer, tokens[0], clientFd);
		return ;
	}
	if (tokens[0].compare("GET") == 0)
		server.getRequest(tokens[1], clientFd);
	else if (tokens[0].compare("DELETE") == 0)
		server.deleteRequest(tokens[1], clientFd);
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
