/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/20 13:51:23 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Manager.hpp"

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
		class						InvalidFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class						SyntaxErrorException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class						OutOfRangeException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
		class						ErrorPageException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif