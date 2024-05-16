/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 16:01:58 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/16 16:12:51 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerManager.hpp"

class Config
{
	private:
		Config(void);
		Config(const Config& original);
		Config&	operator=(const Config& original);
	public:
		Config(const char* filename);
		~Config(void);
		class	ConfigFileException : public std::exception
		{
			public:
				const char*	what(void) const throw();
		};
};

#endif