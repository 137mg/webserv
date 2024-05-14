/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   deleteRequest.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/14 16:07:25 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::deleteRequest(std::string file)
{
	std::string	fullFilePath = "";
	int			result;

	fullFilePath = "cgi-bin/uploads" + file;
	std::cout << fullFilePath << std::endl;
	result = std::remove(fullFilePath.c_str());
	if (result == 0)
	{
		std::cout << "File succesfully deleted" << std::endl;
	}
	else
	{
		perror("Error deleting file");
	}
	std::string response = serveFile("html/files.html", "200 OK", GREEN);
    write(this->_clientFd, response.c_str(), response.size());
    terminalMessage("Server response ", response);
}
