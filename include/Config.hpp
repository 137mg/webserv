/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/21 17:47:36 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerManager.hpp"
# include "Server.hpp"

class	Config
{
	private:
		std::string			_fileContent;
		t_location			_defaultLocation;
		size_t				findOpenBracket(size_t begin);
		size_t				findCloseBracket(size_t begin);
		void				addServer(size_t begin, size_t end);
	public:
		std::vector<Server>	vServers;
		Config(const char* filename);
		~Config(void);
		void				parseFile(void);
		class				ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif