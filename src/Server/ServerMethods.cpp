/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethods.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/06 14:12:41 by mgoedkoo         ###   ########.fr       */
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
			response = serveFile("html/error_pages/PageNotFound.html", "404 Not Found");		
	}
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(void)
{
	std::string	fullFilePath;
	std::string	response;

	fullFilePath = "cgi-bin/uploads" + _header.file;
	// need to decide what to do with delete error!
	if (std::remove(fullFilePath.c_str()) != 0)
		perror("Error deleting file");
	response = serveFile("html/files.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
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
	{
		response = serveFile("html/error_pages/Forbidden.html", "403 Forbidden");
		write(_clientFd, response.c_str(), response.size());
		serverMessage(response, _clientFd, GREEN);
	}
}
