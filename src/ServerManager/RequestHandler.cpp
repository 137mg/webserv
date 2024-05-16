/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/16 15:45:00 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "CGI.hpp"

void	ServerManager::getRequest(std::string file)
{
	std::string filePath = "";
	std::string response = "";

	if (file.compare("/") == 0)
		file = "/home.html";
	if (file.find(".html") != std::string::npos)
		filePath = "html" + file;
	else
		filePath = file.erase(0, 1);
	if (filePath.compare("html/files.html") == 0)
		response = showUploads(filePath, "200 OK", GREEN);
	else
	{
		if (fileAccess(filePath))
			response = serveFile(filePath, "200 OK", GREEN);
		else
			response = serveFile("html/PageNotFound.html", "404 Not Found", RED);		
	}
	write(this->_clientFd, response.c_str(), response.size());
	terminalMessage("ServerManager response ", response);
}

void	ServerManager::deleteRequest(std::string file)
{
	std::string	fullFilePath = "";
	int			result;

	fullFilePath = "cgi-bin/uploads" + file;
	result = std::remove(fullFilePath.c_str());
	if (result != 0)
	{
		perror("Error deleting file");
	}
	std::string response = serveFile("html/files.html", "200 OK", GREEN);
    write(this->_clientFd, response.c_str(), response.size());
    terminalMessage("ServerManager response ", response);
}

void	ServerManager::postRequest(std::string buffer)
{
    CGI cgi;

    cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"));
    cgi.convertVector();
    cgi.executeScript(buffer);
    std::string response = serveFile("html/home.html", "200 OK", GREEN);
    write(this->_clientFd, response.c_str(), response.size());
    terminalMessage("ServerManager response ", response);
}
