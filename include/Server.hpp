/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/26 18:06:12 by mirjam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerManager.hpp"

typedef struct s_location
{
	bool						autoindex;
	std::string					match;
	std::string					root;
	std::string					index;
	std::string					redirect;
	std::vector<std::string>	allowedMethods;
	std::vector<std::string>	cgiExtents;
	std::vector<std::string>	cgiPaths;
	// struct s_location&			operator=(const struct s_location& original);
}	t_location;

// 1) check host?
// 2) set limit to client_max_body_size
// 3) figure out redirects
// 4) figure out cgi extents + paths
// 5) add "PUT" method?

class	Server
{
	private:
		void								initDefaultLocation(void);
		void								initErrorPages(void);
		bool								checkLocation(t_location location);
	public:
		uint16_t							port;
		uint32_t							clientMaxBodySize;
		std::string							host;
		t_location							defaultLocation;
		std::vector<std::string>			serverNames;
		std::deque<t_location>				locations;
		std::map<uint16_t, std::string>		errorPages;
		Server(void);
		Server(const Server& original);
		~Server(void);
		Server&								operator=(const Server& original);
		bool								checkServer(void);
};

#endif