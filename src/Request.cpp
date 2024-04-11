/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Request.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/11 17:38:30 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/11 17:38:48 by juvan-to      ########   odam.nl         */
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
	return;	
}
