/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/18 14:41:06 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 16:50:26 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

void	clientMessage(const std::string &message, int clientFd)
{
	size_t		position;
	std::string	header;

	position = std::string(message).find('\n');
	header = std::string(message).substr(0, position);
	printTimestamp();
	std::cout << RESET << GREEN << "Client request " << RESET << "from socket " << clientFd << "	" << header << std::endl;
}

void	serverMessage(const std::string &message, int clientFd, std::string color)
{
	size_t spacePos = message.find(' ');
    std::string httpVersion = message.substr(0, spacePos);
    size_t nextPos = message.find('\n', spacePos + 1);
    std::string responseCode = message.substr(spacePos + 1, nextPos - spacePos - 1);

	printTimestamp();
	std::cout << RESET << YELLOW << "Server response " << RESET
			<< "to socket " << clientFd << "	" << httpVersion << " "
			<< color << responseCode << RESET << std::endl;
}

void	printTimestamp(void)
{
	std::time_t	result = std::time(nullptr);
	std::tm		*ltm = std::localtime(&result);

	std::cout << "[" << std::put_time(ltm, "%d-%m-%Y  %H:%M:%S") << "]" << "  ";

}
