/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 15:02:40 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "CGI.hpp"

void	ServerManager::getRequest(std::string file, int clientFd)
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
	write(clientFd, response.c_str(), response.size());
	terminalMessage("Server response ", response, clientFd);
}

void	ServerManager::deleteRequest(std::string file, int clientFd)
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
	write(clientFd, response.c_str(), response.size());
	terminalMessage("Server response ", response, clientFd);
}

void	ServerManager::postRequest(std::string buffer, std::string method, int clientFd)
{
    CGI cgi;
	
    cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"), method);
    cgi.convertVector();
    cgi.executeScript(buffer);
    std::string response = serveFile("html/home.html", "200 OK", GREEN);
    write(clientFd, response.c_str(), response.size());
    terminalMessage("Server response ", response, clientFd);
}

// Returns the value of a specific header
std::string	ServerManager::getHeader(std::string buffer, std::string key)
{
	size_t keyPos = buffer.find(key + ":");
	
    if (keyPos == std::string::npos)
	{
        // Key not found in the buffer
        return "";
    }

    // Find the end of the line containing the key
    size_t endOfLinePos = buffer.find("\r\n", keyPos);
    if (endOfLinePos == std::string::npos)
	{
        // End of line not found
        return "";
    }

    // Extract the value after the key
    size_t valueStartPos = keyPos + key.length() + 2; // Skip ": " after the key
    std::string value = buffer.substr(valueStartPos, endOfLinePos - valueStartPos);
    return value;
}
