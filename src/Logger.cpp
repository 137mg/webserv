/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:41:06 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 15:44:10 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

void	terminalMessage(const std::string &s1, const std::string &s2, int clientFd)
{
	size_t		position;
	std::string	header;
	std::size_t	found = s1.find("Client");

	position = std::string(s2).find('\n');
	header = std::string(s2).substr(0, position);
	
	printTimestamp();
	if (found != std::string::npos)
		std::cout << RESET << GREEN << s1 << RESET << "from socket " << clientFd << "	" << header << std::endl;
	else
		std::cout << RESET << YELLOW << s1 << RESET << "to socket " << clientFd << "	" << header << std::endl;
}

void	printTimestamp(void)
{
	std::time_t	result = std::time(nullptr);
	std::tm		*ltm = std::localtime(&result);

	std::cout << "[" << std::put_time(ltm, "%d-%m-%Y  %H:%M:%S") << "]" << "  ";

}
