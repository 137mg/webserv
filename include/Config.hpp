/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/23 14:47:21 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerManager.hpp"
# include "Server.hpp"

class	Config
{
	private:
		std::string			_line;
		std::ifstream		_ifs;
		std::vector<Server>	_servers;
	public:
		Config(const char* filename);
		~Config(void);
		void				removeWhitespace(std::string& line);
		void				parseFile(void);
		void				addServer(void);
		void				addLocation(void);
		class				ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif