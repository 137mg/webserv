/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:49:54 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/18 02:04:40 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

volatile bool RUNNING;

void	signalHandler(int signum)
{
	RUNNING = false;
	std::cout << "Interrupt signal (" << signum << ") received from the user.\n";
}

void	setUpSignals()
{
	RUNNING = true;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
}

int main(int argc, char** argv)
{
	ServerManager	serverManager;

	if (argc > 2)
	{
		std::cerr << "Error: too many arguments" << std::endl;
		return (1);
	}
	try
	{
		if (argc == 2)
			serverManager.configFile(argv[1]);
		else
			serverManager.configFile(DEFAULT_PATH);
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	setUpSignals();
	try
	{
		serverManager.config();
		serverManager.setUpPoll();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
