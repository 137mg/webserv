/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Client.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/14 17:00:22 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 14:31:51 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"

std::string serveFile(const std::string& path, const std::string& status, std::string color)
{
    std::ifstream fileStream(path);
    if (!fileStream.is_open()) {
        // Handle file not found or unable to open error
        return "HTTP/1.1 500 Internal Server Error\r\n"
               "Content-Type: text/html\r\n"
               "Content-Length: 44\r\n\r\n"
               "<html><body>500 Internal Server Error</body></html>";
    }

    std::stringstream responseStream;
    std::string fileContents;
    std::string response;

    responseStream << fileStream.rdbuf();
    fileContents = responseStream.str();

	response = "HTTP/1.1 " + status + "\r\n";
    response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
    response += "Content-Type: text/html\r\n\r\n";
    response += fileContents;

    return response;
	std::cout << color << std::endl;
}

void send413Response(int clientFd)
{
	std::string response = serveFile("html/PayloadTooLarge.html", "413 Payload Too Large", RED);
    write(clientFd, response.c_str(), response.length());
	terminalMessage("Server response ", response, clientFd);
}

// Handle the connection with a client, reading incoming data and processing requests
bool	ServerManager::handleClientConnection(int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read;

	bytes_read = read(clientFd, buffer, MESSAGE_BUFFER);
	if (bytes_read == 0)
	{
		printTimestamp();
		std::cout << GREEN << "Client socket " << RESET << clientFd << RED << " closed " << RESET << "the connection." << std::endl;
		this->_clientBuffers.erase(clientFd);
		return false;
	}
	// still need to decide between exception and error page or the like!
	else if (bytes_read < 0)
	{
		std::cerr << RED << BOLD << "Read error " << std::strerror(errno) << RESET << std::endl;
		return false;
	}
	else
	{
		std::string& clientBuffer = this->_clientBuffers[clientFd];
		clientBuffer.append(buffer, bytes_read);
		// please dont look at the next 10 lines too closely uwu
		if (clientBuffer.size() > 1048576)
		{
			send413Response(clientFd);
            this->_clientBuffers.erase(clientFd);
            close(clientFd);
            return false;
		}
		else if (isRequestComplete(clientBuffer))
		{
			selectServer(clientBuffer, clientFd);
			this->_clientBuffers.erase(clientFd);
		}
	}
	return true;
}

bool ServerManager::isRequestComplete(const std::string &request_buffer)
{
	size_t	headerEnd;
	size_t	totalExpectedSize;
	size_t	contentLengthPos;

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

// Returns the size of a request
size_t ServerManager::getRequestSize(std::string request_buffer)
{
	// Check if Content-Length header exists
	size_t contentLengthPos = request_buffer.find("Content-Length:");
	if (contentLengthPos != std::string::npos)
	{
		size_t contentLengthEnd = request_buffer.find("\r\n", contentLengthPos);
		size_t contentLength = std::stoi(request_buffer.substr(contentLengthPos + 15, contentLengthEnd - contentLengthPos - 15));
		size_t totalExpectedSize = request_buffer.find("\r\n\r\n") + 4 + contentLength;
		return totalExpectedSize;
	}
	return request_buffer.size();
}

void	ServerManager::closeClientConnection(unsigned long i)
{
	printTimestamp();
    std::cout << RED << "Closing " << RESET << "client socket " << RESET << _pollFdsVector[i].fd << std::endl;
    close(_pollFdsVector[i].fd);
    delFromPollFds(i);
}
