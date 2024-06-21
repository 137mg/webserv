/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   SendResponse.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/21 13:21:13 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/21 14:17:59 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"
#include "Server.hpp"

void	Manager::closeClientConnection(unsigned long i)
{
	printTimestamp();
    std::cout << RED << "Closing " << RESET << "client socket " << RESET << _pollFdsVector[i].fd << std::endl;
    close(_pollFdsVector[i].fd);
    delFromPollFds(i);
}

void	Manager::sendResponse(std::string response, int clientFd)
{
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, GREEN);
}

void	Manager::sendErrorResponse(std::string response, int clientFd)
{
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, RED);
}

void	Manager::sendPendingResponse(int clientFd)
{
	if (clientResponses.count(clientFd) > 0 && !clientResponses[clientFd].empty())
    {
        sendResponse(clientResponses[clientFd], clientFd);
        clearFdForWriting(clientFd);
		clientResponses.erase(clientFd);
    }
    else if (clientErrorResponses.count(clientFd) > 0 && !clientErrorResponses[clientFd].empty())
    {
        sendErrorResponse(clientErrorResponses[clientFd], clientFd);
        clearFdForWriting(clientFd);
		clientErrorResponses.erase(clientFd);
    }
}
