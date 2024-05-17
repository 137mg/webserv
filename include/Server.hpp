/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/17 12:31:57 by mirjam           ###   ########.fr       */
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
		void								initErrorPages(void);
	public:
		uint16_t							port;
		uint32_t							clientMaxBodySize;
		std::vector<std::string>			serverNames;
		std::map<uint16_t, std::string>		errorPages;
		std::map<std::string, t_location>	locations;
		Server(void);
		~Server(void);
};

#endif