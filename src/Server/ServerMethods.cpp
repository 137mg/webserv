/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerMethods.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/18 17:14:34 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"

void	Server::getMethod(void)
{
	std::string	filePath;
	std::string	response;

	filePath = _location.root + _header.file;
	if (filePath.back() == '/' && !_location.index.empty())
		filePath += _location.index;
	if (access(filePath.c_str(), R_OK) != 0 || (filePath.back() == '/' && !_location.autoIndex))
	{
		sendErrorResponse(404);
		return;
	}
	if (filePath.back() == '/')
	{
		runCGI("./cgi-bin/directoryListing.py");
		return;
	}
	for (size_t i = 0; i < _location.cgiExtents.size(); i++)
	{
		if (_header.file.find(_location.cgiExtents[i]) != std::string::npos)
		{
			runCGI(filePath);
			return;
		}
	}
	_lastPath = filePath;
	this->_response = buildResponse(filePath, "200 OK");
	this->serverManager->clientResponses[this->_clientFd] = this->_response;
	// response = buildResponse(filePath, "200 OK");
	// write(_clientFd, response.c_str(), response.size());
	// serverMessage(response, _clientFd, GREEN);
}

void	Server::deleteMethod(void)
{
	std::string	filePath;
	std::string	response;

	filePath = _location.root + _header.file;
	if (access(filePath.c_str(), F_OK) == 0)
	{
		if (access(filePath.c_str(), W_OK) == 0)
		{
			std::remove(filePath.c_str());
			this->_response = buildResponse(_lastPath, "200 OK");
			response = buildResponse(_lastPath, "200 OK");
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
	std::string filePath;
	
	filePath = _location.root + _header.file;
	for (size_t i = 0; i < _location.cgiExtents.size(); i++)
	{
		if (_header.file.find(_location.cgiExtents[i]) != std::string::npos)
		{
			runCGI(filePath);
			return;
		}
	}
	sendErrorResponse(415);
}

void	Server::runCGI(std::string filePath)
{
	CGI			cgi(*this, *(this->serverManager));
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
