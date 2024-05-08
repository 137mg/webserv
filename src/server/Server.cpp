/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:19:41 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/08 14:16:20 by juvan-to      ########   odam.nl         */
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
    printTimestamp();
    std::cout << RED << "Closing server socket " << RESET << std::endl;
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

// Create a socket for the server to listen for incoming connections
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

// Bind the socket to a specific address and port and listen for incoming connections
void	Server::bindSocket(void)
{
	int	status;
	
	status = bind(this->_listenFd, reinterpret_cast<struct  sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
	if (status != 0) {
		std::cerr << "bind error: " << std::strerror(errno) << std::endl;
		return ;
	}

	status = listen(this->_listenFd, BACKLOG);
	if (status != 0) {
		std::cerr << "Listen error " << std::strerror(errno) << std::endl;
		return ;
	}
}

// Keep accepting connections while the server is running
void	Server::run(void)
{
	while (1)
	{
		struct sockaddr_storage	client_addr;
		socklen_t				addr_size;
		
		addr_size = sizeof(client_addr);
		this->_clientFd = accept(this->_listenFd, reinterpret_cast<struct sockaddr *>(&client_addr), &addr_size);
		if (this->_clientFd == -1) {
			std::cerr << RED << "Client fd error: " << std::strerror(errno) << RESET << std::endl;
			exit(1);
		}
		fcntl(this->_clientFd, F_SETFL, O_NONBLOCK);
		this->handleClientConnection();
		printTimestamp();
		std::cout << RED << "Closing client socket " << RESET << this->_clientFd << std::endl;
		close(this->_clientFd);
	}
	return;
}

// Handle the connection with a client, reading incoming data and processing requests
void Server::handleClientConnection(void)
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
