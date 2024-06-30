/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerMethods.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 15:11:58 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/30 02:31:11 by Julia         ########   odam.nl         */
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
		buildErrorResponse(404);
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
			buildErrorResponse(403);
	}
	else
		buildErrorResponse(404);
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
	buildErrorResponse(415);
}

void	Server::runCGI(std::string filePath)
{
	CGI			cgi(*this, *(this->Manager));
	std::string	CGIdirectory;
	std::string	CGIfile;

	CGIdirectory = _location.root + _location.match;
	CGIfile = filePath.substr(filePath.rfind("/") + 1);

	cgi.initEnvp(_header, _request);
	cgi.convertVector();
	cgi.errorResponse = buildResponse(errorPages[500], errorMessages[500]);
	if (access(filePath.c_str(), X_OK) == 0)
	{
		cgi.executeScript(CGIfile, CGIdirectory, _request, _clientFd);
	}
	else
		buildErrorResponse(403);
}
