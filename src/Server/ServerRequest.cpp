/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerRequest.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 16:54:16 by juvan-to      ########   odam.nl         */
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
	return (path);
}

void	Server::parseRequest(std::string buffer, int clientFd)
{
	std::vector<std::string>	tokens;
	std::string					token;
	std::istringstream			iss(buffer);
	std::string					requestedPath = findPath(buffer);

	// std::cout << std::endl;
	// std::cout << "---------------- BEGIN BUFFER --------------" << std::endl;
	// std::cout << buffer;
	// std::cout << "---------------- END BUFFER -----------------" << std::endl;
	// std::cout << std::endl;
	_clientFd = clientFd;
	clientMessage(buffer, clientFd);
	if (buffer.size() > 1048576)
	{
		send413Response(clientFd);
		return;
	}
	while (std::getline(iss, token, ' '))
		tokens.push_back(token);
	if (tokens[0] == "POST")
		postMethod(buffer);
	else if (tokens[0] == "GET")
		getMethod(tokens[1], locations[0]);
	else if (tokens[0] == "DELETE")
		deleteMethod(tokens[1]);
}
