/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethods.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/05 15:30:56 by mirjam           ###   ########.fr       */
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
	if (_header.file.find(".html") != std::string::npos)
		filePath = "html" + _header.file;
	else
		filePath = _header.file.erase(0, 1);
	if (filePath == "html/files.html")
		response = showUploads(filePath, "200 OK", GREEN);
	else
	{
		if (access(filePath.c_str(), R_OK) == 0)
			response = serveFile(filePath, "200 OK");
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
	CGI			cgi;
	std::string	response;

	cgi.initEnvp(_header.contentType, _header.contentLength, _header.method);
	cgi.convertVector();
	cgi.executeScript(_request);
	response = serveFile("html/home.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}
