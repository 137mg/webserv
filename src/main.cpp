/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:49:54 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/15 19:47:50 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char** argv)
{
	Server	server;

	if (argc > 2)
	{
		std::cerr << "Error: too many arguments" << std::endl;
		return (1);
	}
	try
	{
		if (argc == 2)
			server.configFile(argv[1]);
		else
			server.configFile(DEFAULT_PATH);
		server.config();
		server.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::strerror(errno) << std::endl;
		return (1);
	}
	return (0);
}
