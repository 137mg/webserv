<<<<<<< HEAD
<<<<<<< HEAD
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trossel <trossel@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 15:21:50 by trossel           #+#    #+#             */
/*   Updated: 2022/10/09 16:58:18 by trossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Looper.hpp"
#include "ConfigParser.hpp"

void stopLoop(int sig)
{
    RUNNING = 0;
    std::cout << RED << "Signal " << sig << " catched. Loop is being stopped." << RESET << std::endl;
}

void setupSignals()
{
    RUNNING = 1;
    signal(SIGINT, stopLoop);
}

Config getConfig(const std::string &filename)
{
	ConfigParsor parsor(filename);
	Config conf = parsor.parse();
	return conf;
}

int	main(int argc, char *argv[])
{
	if (argc < 2 || argc > 2)
	{
		std::cerr << "Usage: ./webserv <config file>" << std::endl;
		exit(1);
	}
	Config cfg = getConfig(argv[1]);
	if (!cfg.isValid())
	{
		std::cerr << "Error in config file." << std::endl;
		return 1;
	}
    setupSignals();

    Looper loop(cfg);

    if (loop.setupLoop() == -1)
        return (1);

    loop.loop();
    
    return (0);
}
=======
=======
>>>>>>> 4fda795 (cleanup)
#include "../inc/Webserv.hpp"
#include "../inc/ServerManager.hpp"

void sigpipeHandle(int sig) { if(sig) {}}

int main(int argc, char **argv) 
{
	// Logger::setState(OFF);
	if (argc == 1 || argc == 2) {
		try 
		{
			std::string		config;
			ConfigParser	cluster;
        	ServerManager 	master;
			signal(SIGPIPE, sigpipeHandle);
			/* configuration file as argument or default path */
			config = (argc == 1 ? "configs/default.conf" : argv[1]);
			cluster.createCluster(config);
			// cluster.print(); // for checking
			master.setupServers(cluster.getServers());
			master.runServers();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			return (1);
		}
    }
    else 
	{
		Logger::logMsg(RED, CONSOLE_OUTPUT, "Error: wrong arguments");
		return (1);
	}
    return (0);
}

<<<<<<< HEAD
>>>>>>> 8b3d215 (add examples)
=======
=======
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trossel <trossel@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/20 15:21:50 by trossel           #+#    #+#             */
/*   Updated: 2022/10/09 16:58:18 by trossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Looper.hpp"
#include "ConfigParser.hpp"

void stopLoop(int sig)
{
    RUNNING = 0;
    std::cout << RED << "Signal " << sig << " catched. Loop is being stopped." << RESET << std::endl;
}

void setupSignals()
{
    RUNNING = 1;
    signal(SIGINT, stopLoop);
}

Config getConfig(const std::string &filename)
{
	ConfigParsor parsor(filename);
	Config conf = parsor.parse();
	return conf;
}

int	main(int argc, char *argv[])
{
	if (argc < 2 || argc > 2)
	{
		std::cerr << "Usage: ./webserv <config file>" << std::endl;
		exit(1);
	}
	Config cfg = getConfig(argv[1]);
	if (!cfg.isValid())
	{
		std::cerr << "Error in config file." << std::endl;
		return 1;
	}
    setupSignals();

    Looper loop(cfg);

    if (loop.setupLoop() == -1)
        return (1);

    loop.loop();
    
    return (0);
}
>>>>>>> e4d723c (cleanup)
>>>>>>> 4fda795 (cleanup)
