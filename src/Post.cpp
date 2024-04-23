/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Post.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: Julia <Julia@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/23 23:05:36 by Julia         #+#    #+#                 */
/*   Updated: 2024/04/24 01:05:53 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::postRequest(std::string path)
{
	std::string response = "";
	std::string hardcoded_path = "./cgi-bin/upload.py";

	// execve cgi

	response = serveFile("html/home.html", "200 OK", GREEN);
	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("Server response to ", response);
	std::cout << path << std::endl;
}
