/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 18:08:58 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerManager.hpp"

typedef struct s_location
{
	std::vector<std::string>	allowedMethods;
	std::vector<std::string>	cgiExtents;
	std::vector<std::string>	cgiPaths;
	std::string					redirect;
	std::string					root;
	std::string					index;
	bool						autoindex = false;
}	t_location;

class	Server
{
	private:
		// uint16_t							_port;
		// uint16_t							_clientMaxBodySize;
		std::vector<std::string>			_serverNames;
		std::map<uint16_t, std::string>		_errorPages;
		std::map<std::string, t_location>	_locations;
	public:
		Server(void);
		~Server(void);
};

#endif