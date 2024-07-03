/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SendResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:21:13 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/03 01:16:26 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

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
	if (clientResponses.count(clientFd) > 0 && !clientResponses[clientFd].empty())
    {
        sendResponse(clientResponses[clientFd], clientFd);
		clientResponses.erase(clientFd);
		_clientStatus[clientFd] = READING;
        markFdForReading(clientFd);
    }
    else if (clientErrorResponses.count(clientFd) > 0 && !clientErrorResponses[clientFd].empty())
    {
        if (sendErrorResponse(clientErrorResponses[clientFd], clientFd))
            markFdForReading(clientFd);
		clientErrorResponses.erase(clientFd);
		_clientStatus[clientFd] = READING;
    }
}
