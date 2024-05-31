/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 16:13:28 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static std::string	findPath(const std::string &request)
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

void	Server::parseRequest(std::string buffer, int clientFd)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					requestedPath = findPath(buffer);

	_clientFd = clientFd;
	terminalMessage("Client request ", buffer, clientFd);
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	if (requestedPath.find("cgi-bin/upload.py") != std::string::npos)
	{
		postRequest(buffer, tokens[0]);
		return ;
	}
	if (tokens[0].compare("GET") == 0)
		getRequest(tokens[1]);
	else if (tokens[0].compare("DELETE") == 0)
		deleteRequest(tokens[1]);
	return;
}
