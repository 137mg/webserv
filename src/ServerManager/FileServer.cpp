/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 16:41:56 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/30 14:57:25 by psadeghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

std::string	ServerManager::serveFile(const std::string &path, const std::string &status, const std::string &color)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	responseStream << fileStream.rdbuf();
	
	std::string fileContents = responseStream.str();
	std::string response = "HTTP/1.1 " + color + status + RESET + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	// response += "Connection: keep-alive\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
		response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return response;
}

bool	ServerManager::fileAccess(const std::string &path)
{
	std::ifstream fileStream(path);
	return fileStream.good();
}
