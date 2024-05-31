/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/31 16:14:05 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ServerManager.hpp"

typedef struct s_location
{
	bool						autoIndex;
	std::string					match;
	std::string					root;
	std::string					index;
	std::string					redirect;
	std::vector<std::string>	allowedMethods;
	std::vector<std::string>	cgiExtents;
	std::vector<std::string>	cgiPaths;
}	t_location;

// check host?
// figure out redirects

class	Server
{
	private:
		int									_clientFd;
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
		
		void	parseRequest(std::string buffer, int clientFd);
		void	getRequest(std::string file);
		void	deleteRequest(std::string file);
		void	postRequest(std::string buffer, std::string method);

		std::string	getHeader(std::string buffer, std::string key);
		std::string	serveFile(const std::string &path, const std::string &status, const std::string &color);
		std::string	showUploads(const std::string &path, const std::string &status, const std::string &color);

};

#endif