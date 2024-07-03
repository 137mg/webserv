/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psadeghi <psadeghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:49:54 by juvan-to          #+#    #+#             */
/*   Updated: 2024/07/03 17:29:38 by psadeghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

volatile bool RUNNING;

void	signalHandler(int signum)
{
	RUNNING = false;
	if (signum == 2)
		std::cout << std::endl << "Shutting down the server. Goodbye!" << std::endl;
	else
		std::cout << "Interrupt signal (" << signum << ") received from the user." << std::endl;
}

void	setUpSignals(void)
{
	RUNNING = true;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

int	main(int argc, char** argv)
{
	Manager	Manager;

	if (argc > 2)
	{
		std::cerr << "Error: too many arguments" << std::endl;
		return (1);
	}
	try
	{
		if (argc == 2)
			Manager.configFile(argv[1]);
		else
			Manager.configFile("config_files/default.toml");
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}
	setUpSignals();
	try
	{
		Manager.startServer();
		Manager.monitorSockets();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
