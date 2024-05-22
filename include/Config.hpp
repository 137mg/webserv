/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 17:17:21 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerManager.hpp"
# include "Server.hpp"

class	Config
{
	private:
		std::ifstream	_ifs;
		Config(void);
		Config(const Config& original);
		Config&			operator=(const Config& original);
	public:
		Config(const char* filename);
		~Config(void);
		class			ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif