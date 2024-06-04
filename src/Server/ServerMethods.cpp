/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerMethods.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 16:48:46 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

void	Server::getMethod(std::string file, t_location location)
{
	std::string	filePath;
	std::string	response;

	if (file == "/")
		file += location.index;
	if (file.find(".html") != std::string::npos)
		filePath = "html" + file;
	else
		filePath = file.erase(0, 1);
	if (filePath == "html/files.html")
		response = showUploads(filePath, "200 OK", GREEN);
	else
	{
		if (access(filePath.c_str(), R_OK) == 0)
			response = serveFile(filePath, "200 OK");
		else
			response = serveFile("html/PageNotFound.html", "404 Not Found");		
	}
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(std::string file)
{
	std::string	fullFilePath;
	std::string	response;

	fullFilePath = "cgi-bin/uploads" + file;
	if (std::remove(fullFilePath.c_str()) != 0)
		perror("Error deleting file");
	response = serveFile("html/files.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::postMethod(std::string buffer)
{
	CGI	cgi;

	cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"), "POST");
	cgi.convertVector();
	cgi.executeScript(buffer);
	std::string response = serveFile("html/home.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

// Returns the value of a specific header
std::string	Server::getHeader(std::string buffer, std::string key)
{
	size_t keyPos = buffer.find(key + ":");
	
	if (keyPos == std::string::npos)
	{
		// Key not found in the buffer
		return ("");
	}
	// Find the end of the line containing the key
	size_t endOfLinePos = buffer.find("\r\n", keyPos);
	if (endOfLinePos == std::string::npos)
	{
		// End of line not found
		return ("");
	}
	// Extract the value after the key
	size_t valueStartPos = keyPos + key.length() + 2; // Skip ": " after the key
	std::string value = buffer.substr(valueStartPos, endOfLinePos - valueStartPos);
	return (value);
}
