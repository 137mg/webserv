/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethods.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 16:14:38 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

void	Server::getRequest(std::string file)
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
		if (access(filePath.c_str(), R_OK) == 0)
			response = serveFile(filePath, "200 OK", GREEN);
		else
			response = serveFile("html/PageNotFound.html", "404 Not Found", RED);		
	}
	write(_clientFd, response.c_str(), response.size());
	terminalMessage("Server response ", response, _clientFd);
}

void	Server::deleteRequest(std::string file)
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
	write(_clientFd, response.c_str(), response.size());
	terminalMessage("Server response ", response, _clientFd);
}

void	Server::postRequest(std::string buffer, std::string method)
{
    CGI cgi;
	
    cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"), method);
    cgi.convertVector();
    cgi.executeScript(buffer);
    std::string response = serveFile("html/home.html", "200 OK", GREEN);
    write(_clientFd, response.c_str(), response.size());
    terminalMessage("Server response ", response, _clientFd);
}

// Returns the value of a specific header
std::string	Server::getHeader(std::string buffer, std::string key)
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

std::string	Server::serveFile(const std::string &path, const std::string &status, const std::string &color)
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
