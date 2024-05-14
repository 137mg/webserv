/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   deleteRequest.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/14 15:16:05 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::deleteRequest(std::string file)
{
	std::cout << file << std::endl;
	std::string response = serveFile("html/files.html", "200 OK", GREEN);
    write(this->_clientFd, response.c_str(), response.size());
    terminalMessage("Server response ", response);
}
