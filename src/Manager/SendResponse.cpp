/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SendResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:21:13 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/04 14:26:04 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

void	Manager::sendResponse(std::string response, int clientFd)
{
	ssize_t			bytesWritten;

	bytesWritten = write(clientFd, response.c_str(), response.size());
	if (bytesWritten < 0)
	{
		closeClientConnection(clientFd);
		return;
	}
	else if (bytesWritten == 0)
		return;
	serverMessage(response, clientFd, GREEN);
}

bool	Manager::sendErrorResponse(std::string response, int clientFd)
{
	std::string		errorLine;
	ssize_t			bytesWritten;


	bytesWritten = write(clientFd, response.c_str(), response.size());
	if (bytesWritten < 0)
	{
		closeClientConnection(clientFd);
		return (false);
	}
	else if (bytesWritten == 0)
		return (true);
	serverMessage(response, clientFd, RED);
	errorLine = response.substr(0, response.find("\r\n"));
	if (errorLine == "HTTP/1.1 413 Payload Too Large")
	{
		closeClientConnection(clientFd);
		return (false);
	}
	return (true);
}

void	Manager::sendPendingResponse(int clientFd)
{
	if (_clientResponses.count(clientFd) > 0 && !_clientResponses[clientFd].empty())
	{
		sendResponse(_clientResponses[clientFd], clientFd);
		markFdForReading(clientFd);
		_clientResponses.erase(clientFd);
	}
	else if (_clientErrorResponses.count(clientFd) > 0 && !_clientErrorResponses[clientFd].empty())
	{
		if (sendErrorResponse(_clientErrorResponses[clientFd], clientFd))
			markFdForReading(clientFd);
		_clientErrorResponses.erase(clientFd);
	}
}
