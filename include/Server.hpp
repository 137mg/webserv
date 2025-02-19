/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mirjam <mirjam@student.42.fr>                +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/16 17:09:18 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/07/03 16:46:38 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Manager.hpp"

class	Manager;

class	Server
{
	private:
		int				_clientFd;
		std::string		_request;
		std::string		_lastPath;
		t_header		_header;
		t_location		_location;
		

	public:
		Manager								*Manager;
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
		void								checkServer(void);
		void								handleRequest(t_header header, std::string request, int clientFd);
		void								buildErrorResponse(int errorCode);
		void								getMethod(void);
		void								deleteMethod(void);
		void								postMethod(void);
		void								buildRedirectResponse(void);
		void								initDefaultLocation(void);
		void								initErrorPages(void);
		void								checkLocation(t_location location);
		t_location							selectLocation(void);
		int									checkRequest(void);
		void								runCGI(std::string filePath);
		std::string							buildResponse(const std::string path, std::string status);
		class								StandardValuesException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class								LocationSyntaxException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class								LocationPathException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif