/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerMethods.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/11 13:46:06 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

void	Server::getMethod(void)
{
	std::string	filePath;
	std::string	response;

	if (_header.file == "/")
		_header.file += _location.index;
	// Determine the file path based on the file extension
	if (_header.file.find(".html") != std::string::npos)
		filePath = "html" + _header.file;
	else
		filePath = _header.file.erase(0, 1); // erase the leading slash so /index.html becomes index.html
	if (filePath == "html/files.html")
		response = showUploads(filePath, "200 OK", GREEN);
	else
	{
		if (access(filePath.c_str(), R_OK) == 0)
		{
			if (_header.file.find(".py") != std::string::npos)
			{
				runCGI(filePath);
				return;
			}
			else
				response = serveFile(filePath, "200 OK");
		}
		else
		{
			sendErrorResponse(_clientFd, 404);
			return;
		}
	}
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(void)
{
	std::string	filePath;
	std::string	response;

	filePath = _header.file;
	if (!filePath.empty())
		filePath = filePath.substr(1);
	if (access(filePath.c_str(), F_OK) == 0)
	{
		if (access(filePath.c_str(), W_OK) == 0)
		{
			if (std::remove(filePath.c_str()) != 0)
				perror("Error deleting file");
			response = serveFile("html/files.html", "200 OK");
			write(_clientFd, response.c_str(), response.size());
			serverMessage(response, _clientFd, GREEN);
		}
		else
			sendErrorResponse(_clientFd, 403);
	}
	else
		sendErrorResponse(_clientFd, 404);
}

void	Server::postMethod(void)
{
	CGI			cgi(*this);
	std::string	response;

	cgi.initEnvp(_header);
	cgi.convertVector();
	cgi.executeScript("./cgi-bin/upload.py", _request);
	response = serveFile("html/home.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::runCGI(std::string filepath)
{
	CGI			cgi(*this);
	std::string	response;

	cgi.initEnvp(_header);
	cgi.convertVector();
	filepath = "./" + filepath;
	if (access(filepath.c_str(), X_OK) == 0)
	{
		cgi.executeScript(filepath, _request);
		response = serveFile("html/home.html", "200 OK");
		write(_clientFd, response.c_str(), response.size());
		serverMessage(response, _clientFd, GREEN);
	}
	else
		sendErrorResponse(_clientFd, 403);
}
