/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 17:00:22 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/14 17:33:57 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// Handle the connection with a client, reading incoming data and processing requests
void Server::handleClientConnection(void)
{
    std::string request_buffer;
	int			bytes_read;

    while (1)
	{
		bytes_read = readFromSocket(request_buffer);
        if (bytes_read < 0)
        	break;
		else if (bytes_read == 0)
			continue;
		else
		{
            request_buffer.append(this->_buffer);
			if (isRequestComplete(request_buffer))
			{
				handleRequest(request_buffer, bytes_read);
				break;
			}
		}
    }
}

// Read data from the client socket into a buffer
int Server::readFromSocket(std::string &outbuffer)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read = recv(this->_clientFd, buffer, MESSAGE_BUFFER, 0);
	
	if (bytes_read > 0)
		outbuffer.append(buffer, bytes_read);
	else if (bytes_read == 0)
	{
		std::cout << "Client socket " << this->_clientFd << " closed the connection." << std::endl;
        return -1;
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
        	return 0;
        else
		{
            std::cerr << RED << BOLD << "recv error " << std::strerror(errno) << RESET << std::endl;
            return -1;
        }
	}
	return bytes_read;
}

bool Server::isRequestComplete(const std::string &request_buffer)
{
	size_t	headerEnd;
	size_t totalExpectedSize;
	size_t contentLengthPos;

	headerEnd = request_buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;
	totalExpectedSize = getRequestSize(request_buffer);
	contentLengthPos = request_buffer.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		if (request_buffer.size() >= totalExpectedSize)
			return true;
	} 
	else
		return true;
	return false;
}
