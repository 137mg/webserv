/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/01 00:53:57 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{	
	this->_port = PORT;
	this->_serverName = "Webserv";
	this->_root = "";
	this->_index = "";
	this->_listenFd = 0;
	this->_clientMaxBodySize = 1;
	this->_autoIndex = false;
	return;
}

Server::~Server(void)
{
	std::cout << "Closing server socket." << std::endl;
	close(this->_listenFd);
	return;
}

Server::Server(const Server &other)
{
	*this = other;
}

Server & Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->_port = other._port;
		this->_listenFd = other._listenFd;
		this->_clientFd = other._clientFd;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_serverName = other._serverName;
		this->_root = other._root;
		this->_autoIndex = other._autoIndex;
		this->_serverAddress = other._serverAddress;
	}
	return *this;
}

// Step 1: create a socket
void	Server::createSocket(void)
{
	int	opt;

	// sa.sin_family or AF_INET
	this->_listenFd = socket(this->_serverAddress.sin_family , SOCK_STREAM, 0);
	if (this->_listenFd == -1) {
		std::cerr << "Socket fd error = " << std::strerror(errno) << std::endl;
		return;
	}

	// this allows the socket to reuse a local address even if it is already in use
	opt = 1;
    if (setsockopt(this->_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
        std::cerr << "setsockopt error: " << std::strerror(errno) << std::endl;
        close(this->_listenFd);
        return;
    }
}

void	Server::config(void)
{

	this->_serverAddress.sin_family = AF_INET;
	this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
	this->_serverAddress.sin_port = htons(PORT);

	this->createSocket();
	this->bindSocket();
	
	printTimestamp();
	std::cout << this->_serverName << " up and running. Listening on port: "
		<< this->_port << std::endl;
	
}

// Step 2: Identify a socket
void	Server::bindSocket(void)
{
	int	status;
	
	status = bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return ;
	}

	// Step 3: Wait for incoming connection
	status = listen(this->_listenFd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return ;
	}
}

void	Server::acceptConnection(void)
{
	struct sockaddr_storage	client_addr;
	socklen_t				addr_size;

	
	addr_size = sizeof(client_addr);
	this->_clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
	if (this->_clientFd == -1) {
		std::cerr << RED << "client fd error: " << std::strerror(errno) << RESET << std::endl;
		exit(1);
	}

	fcntl(this->_clientFd, F_SETFL, O_NONBLOCK);
	this->processConnection();
}

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

int Server::processRequest(const std::string &request_buffer)
{
	// Check if the request is complete
	size_t headerEnd = request_buffer.find("\r\n\r\n");
	if (headerEnd != std::string::npos)
	{
		// Check if Content-Length header exists
		size_t contentLengthPos = request_buffer.find("Content-Length:");
		if (contentLengthPos != std::string::npos)
		{
			size_t contentLengthEnd = request_buffer.find("\r\n", contentLengthPos);
			size_t contentLength = std::stoi(request_buffer.substr(contentLengthPos + 15, contentLengthEnd - contentLengthPos - 15));
			size_t totalExpectedSize = headerEnd + 4 + contentLength;
			if (request_buffer.size() >= totalExpectedSize)
			{
				// We have received the complete request
				this->handleRequest(request_buffer.c_str(), totalExpectedSize);
				return 1;
			}
		} else
		{
			// No Content-Length header, assume request is complete
			this->handleRequest(request_buffer.c_str(), request_buffer.size());
			return 1;
		}
	}
	return 0;
}

void Server::processConnection(void)
{
    std::string request_buffer;
	int bytes_read;

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
			if (processRequest(request_buffer) == 1)
				break;
		}
    }

    printTimestamp();
    std::cout << "Closing client socket " << this->_clientFd << std::endl;
    close(this->_clientFd);
}


void	Server::run(void)
{
	while (1)
	{
		this->acceptConnection();
	}
	return;
}

std::string	Server::getHeader(std::string buffer, std::string key)
{
	size_t keyPos = buffer.find(key + ":");
    if (keyPos == std::string::npos) {
        // Key not found in the buffer
        return "";
    }

    // Find the end of the line containing the key
    size_t endOfLinePos = buffer.find("\r\n", keyPos);
    if (endOfLinePos == std::string::npos) {
        // End of line not found
        return "";
    }

    // Extract the value after the key
    size_t valueStartPos = keyPos + key.length() + 2; // Skip ": " after the key
    std::string value = buffer.substr(valueStartPos, endOfLinePos - valueStartPos);

    return value;
}