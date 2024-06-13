/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerFiles.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 13:23:18 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/13 14:31:09 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string	Server::serveFile(const std::string path, std::string status)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	std::string			fileContents;
	std::string			response;

	responseStream << fileStream.rdbuf();
	fileContents = responseStream.str();

	response = "HTTP/1.1 " + status + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
		response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return (response);
}

// placeholder
std::string	Server::showDirectoryListing(const std::string path)
{
	return (serveFile(path, "200 OK"));
}

void	Server::sendErrorResponse(int errorCode)
{
	std::string response = serveFile(errorPages[errorCode], errorMessages[errorCode]);
	write(_clientFd, response.c_str(), response.length());
	serverMessage(response, _clientFd, RED);
}
