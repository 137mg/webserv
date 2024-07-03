/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   BuildResponse.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/13 13:23:18 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 13:17:13 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string	Server::buildResponse(const std::string path, std::string status)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	std::string			fileContents;
	std::string			response;

	responseStream << fileStream.rdbuf();
	fileContents = responseStream.str();

	response = "HTTP/1.1 " + status + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
		response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return (response);
}

void	Server::buildRedirectResponse(void)
{
	std::string	response;

	response = "HTTP/1.1 301 Moved Permanently\r\n";
	response += "Location: " + _location.redirect + "\r\n";
	response += "Connection: keep-alive\r\n\r\n";
	this->Manager->setClientResponses(_clientFd, response);
}

void	Server::buildErrorResponse(int errorCode)
{
	std::string	response;

	if (errorCode == 301)
	{
		buildRedirectResponse();
		return;
	}
	response = buildResponse(errorPages[errorCode], errorMessages[errorCode]);
	this->Manager->setClientErrorResponses(_clientFd, response);
}
