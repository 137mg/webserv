/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Logger.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/18 14:41:06 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 15:58:05 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

void	clientMessage(const std::string &s1, const std::string &s2, int clientFd)
{
	size_t		position;
	std::string	header;

	position = std::string(s2).find('\n');
	header = std::string(s2).substr(0, position);
	printTimestamp();
	std::cout << RESET << GREEN << s1 << RESET << "from socket " << clientFd << "	" << header << std::endl;
}

void	serverMessage(const std::string &s1, const std::string &s2, int clientFd)
{
	size_t		position;
	std::string	header;

	position = std::string(s2).find('\n');
	header = std::string(s2).substr(0, position);
	printTimestamp();
	std::cout << RESET << YELLOW << s1 << RESET << "to socket " << clientFd << "	" << header << std::endl;
}

void	printTimestamp(void)
{
	std::time_t	result = std::time(nullptr);
	std::tm		*ltm = std::localtime(&result);

	std::cout << "[" << std::put_time(ltm, "%d-%m-%Y  %H:%M:%S") << "]" << "  ";

}
