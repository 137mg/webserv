/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/09 13:49:54 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/05/22 13:47:51 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

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
		while(1)
		{
			serverManager.config();
			serverManager.preparePoll();
			serverManager.setUpPoll();
	//		serverManager.run();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
