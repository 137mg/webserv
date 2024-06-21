/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMethods.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:11:58 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/21 17:43:53 by psadeghi         ###   ########.fr       */
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
	this->Manager->clientResponses[_clientFd] = buildResponse(filePath, "200 OK");
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
			this->Manager->clientResponses[_clientFd] = buildResponse(filePath, "200 OK");
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
	CGI			cgi(*this, *(this->Manager));

	cgi.initEnvp(_header, _request);
	cgi.convertVector();
	if (access(filePath.c_str(), X_OK) == 0)
	{
		cgi.executeScript(filePath, _request, _clientFd);
	}
	else
		sendErrorResponse(403);
}
