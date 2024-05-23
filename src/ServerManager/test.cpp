bool	ServerManager::handleClientConnection(int clientFd)
{
    std::string request_buffer;
	int			bytes_read;

    while (1)
	{
		bytes_read = readFromSocket(request_buffer, clientFd);
        if (bytes_read < 0)
        	return false;
		else if (bytes_read == 0)
			continue;
		else
		{
			request_buffer.append(this->_buffer);
			if (isRequestComplete(request_buffer))
			{
				handleRequest(request_buffer, clientFd);
				break;
			}
		}
	}
	return true;
}

// Read data from the client socket into a buffer
int ServerManager::readFromSocket(std::string &outbuffer, int clientFd)
{
	char	buffer[MESSAGE_BUFFER];
	int		bytes_read = recv(clientFd, buffer, MESSAGE_BUFFER, 0);
	int flags = fcntl(clientFd, F_GETFL, 0);
	
	if (bytes_read > 0)
		outbuffer.append(buffer, bytes_read);
	else if (bytes_read == 0)
	{
		printTimestamp();
		std::cout << GREEN << "Client socket " << RESET << clientFd << RED << " closed " << RESET << "the connection." << std::endl;
		return -1;
	}
	else < 0
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


void	ServerManager::setUpPoll(void)
{
	this->_pollFds[0].fd = this->_listenFd;
	this->_pollFds[0].events = POLLIN;
	this->_pollCount = 1;

	while (true)
	{
		this->_status = poll(this->_pollFds, this->_pollCount, 2000);
		if (this->_status == -1) {
			std::cerr << RED << "[Server] Poll error: " << std::strerror(errno) << std::endl;
			throw ServerSocketException();
		}
		else if (this->_status == 0)
			continue;
		for (int i = 0; i < this->_pollCount; i++)
		{
			if ((this->_pollFds[i].revents & POLLIN))
			{
				if (this->_pollFds[i].fd == this->_listenFd)
					this->run();

				else
				{
					std::string outbuffer;
                    int result = this->readFromSocket(outbuffer, this->_pollFds[i].fd);
                    if (result > 0) {
						handleRequest(outbuffer, this->_pollFds[i].fd);

                        // Process the data read from the socket
                        // std::cout << "Read " << result << " bytes: " << outbuffer << std::endl;
                    } else if (result == 0) {
                        // Connection closed or no data available
                        std::cout << RED << "Closing client socket " << this->_pollFds[i].fd << RESET << std::endl;
                        close(this->_pollFds[i].fd);
                        this->delFromPollFds(i);
                    } else {
                        // An error occurred
                        std::cerr << RED << "Error reading from client socket " << this->_pollFds[i].fd << RESET << std::endl;
                        close(this->_pollFds[i].fd);
                        this->delFromPollFds(i);
                    }
				}

				// else if (!this->handleClientConnection(this->_pollFds[i].fd))
				// {
				// 	printTimestamp();
				// 	std::cout << RED << "Closing " << RESET << "client socket " << RESET << this->_pollFds[i].fd << std::endl;
				// 	close(this->_pollFds[i].fd);
				// 	this->delFromPollFds(i);
				// }
			}
			else if (this->_pollFds[i].revents & (POLLHUP | POLLERR))
			{
				printTimestamp();
				std::cout << RED << "Closing " << RESET << "client socket " << RESET << this->_pollFds[i].fd << std::endl;
				close(this->_pollFds[i].fd);
				this->delFromPollFds(i);
			}
		}
	}
}