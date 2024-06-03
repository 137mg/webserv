/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/06/03 13:39:20 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerManager.hpp"
# include "Server.hpp"

// check host, locations, allowed methods, cgi settings, etc.

class	Config
{
	private:
		std::string					_line;
		std::string					_key;
		std::vector<std::string>	_values;
		std::ifstream				_ifs;
		void						removeWhitespace(void);
		void						parseLine(void);
		void						parseArray(const std::string& valueStr);
		uint16_t					stouint16(const std::string& str);
		bool						newTable(Server& server);
		void						addServer(void);
		void						addErrorPages(Server& server);
		void						addLocation(Server& server);
		void						updateServer(Server& server);
		void						updateErrorPages(Server& server);
		void						updateLocation(t_location& location);
		void						updateVector(std::vector<std::string>& vector);
	public:
		std::deque<Server>	servers;
		Config(const char* filename);
		~Config(void);
		void						parseFile(void);
		void						printServers(void);
		class						ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif