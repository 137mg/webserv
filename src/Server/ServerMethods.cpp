/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerMethods.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/12 17:31:40 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

void	Server::getMethod(void)
{
	std::string	filePath;
	std::string	response;

	filePath = _location.root + _header.file;
	if (filePath.back() == '/')
	{
		if (!_location.autoIndex)
			filePath += _location.index;
		// else the contents of the directory need to be shown
	}
	// if (filePath == "./html/files.html")
	// 	response = showUploads(filePath, "200 OK", GREEN);
	// else
	// {
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
		sendErrorResponse(404);
		return;
	}
	// }
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(void)
{
	std::string	filePath;
	std::string	response;

	filePath = _location.root + _header.file;
	
	// std::cout << "file: " << _header.file << std::endl;
	// std::cout << "path: " << filePath << std::endl;
	
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
			sendErrorResponse(403);
	}
	else
		sendErrorResponse(404);
}

void	Server::postMethod(void)
{
	CGI			cgi(*this);
	std::string	filePath;
	std::string	response;

	filePath = _location.root + _header.file;
	cgi.initEnvp(_header);
	cgi.convertVector();
	cgi.executeScript(filePath, _request, _clientFd);
	response = serveFile("html/home.html", "200 OK");
	write(_clientFd, response.c_str(), response.size());
	serverMessage(response, _clientFd, GREEN);
}

void	Server::runCGI(std::string filePath)
{
	CGI			cgi(*this);
	std::string	response;

	cgi.initEnvp(_header);
	cgi.convertVector();
	if (access(filePath.c_str(), X_OK) == 0)
	{
		cgi.executeScript(filePath, _request, _clientFd);
	}
	else
		sendErrorResponse(403);
}
