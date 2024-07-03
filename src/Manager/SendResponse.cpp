/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/21 13:21:13 by juvan-to          #+#    #+#             */
/*   Updated: 2024/07/03 15:01:18 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

void	Manager::sendResponse(std::string response, int clientFd)
{
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, GREEN);
}

bool	Manager::sendErrorResponse(std::string response, int clientFd)
{
	std::string errorLine;

	write(clientFd, response.c_str(), response.size());
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
		_clientStatus[clientFd] = READING;
	}
	else if (_clientErrorResponses.count(clientFd) > 0 && !_clientErrorResponses[clientFd].empty())
	{
		if (sendErrorResponse(_clientErrorResponses[clientFd], clientFd))
			markFdForReading(clientFd);
		_clientErrorResponses.erase(clientFd);
		_clientStatus[clientFd] = READING;
	}
}
