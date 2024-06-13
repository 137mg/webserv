/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/13 14:14:54 by mgoedkoo         ###   ########.fr       */
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

typedef struct errorPageInfo
{
    std::string path;
    std::string message;
}	t_errorPageInfo;

// check host?
// check server names?
// check locations more thoroughly?
// figure out redirects
// implement cgiExtents, cgiPaths

class	Server
{
	private:
		int			_clientFd;
		std::string	_request;
		t_header	_header;
		t_location	_location;
		void		initDefaultLocation(void);
		void		initErrorPages(void);
		bool		checkLocation(t_location location);
		t_location	selectLocation(void);
		void		getMethod(void);
		void		deleteMethod(void);
		void		postMethod(void);
		void		sendErrorResponse(int errorCode);
		void		runCGI(std::string filePath);
		std::string	serveFile(const std::string path, std::string status);
		std::string	showDirectoryListing(const std::string path);

	public:
		uint16_t							port;
		uint32_t							clientMaxBodySize;
		std::string							host;
		t_location							defaultLocation;
		std::vector<std::string>			serverNames;
		std::deque<t_location>				locations;
		std::map<uint16_t, std::string>		errorPages;
		std::map<uint16_t, std::string>		errorMessages;
		Server(void);
		Server(const Server& original);
		~Server(void);
		Server&								operator=(const Server& original);
		bool								checkServer(void);
		void								handleRequest(t_header header, std::string request, int clientFd);
};

#endif