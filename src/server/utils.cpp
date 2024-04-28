/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/18 14:41:06 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/28 23:27:53 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>

void	printTimestamp(void)
{
	std::time_t	result = std::time(nullptr);
	std::tm		*ltm = std::localtime(&result);

	std::cout << "[" << std::put_time(ltm, "%d-%m-%Y  %H:%M:%S") << "]" << "  ";

}
