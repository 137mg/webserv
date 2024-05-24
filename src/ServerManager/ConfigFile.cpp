/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 17:42:09 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/24 17:58:46 by mirjam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Config.hpp"

void	ServerManager::configFile(const char* filename)
{
	errno = EINVAL;
	Config	config(filename);
	config.parseFile();
	// add servers to ServerManager
	errno = 0;
}
