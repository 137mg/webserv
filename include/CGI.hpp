/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 14:52:49 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/06 14:02:34 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "ServerManager.hpp"

class CGI
{
	private:
		char						**_envp;
		std::vector<std::string>	_envpVector;
		Server						_server;
	
	public:
		CGI(const Server& server);
		~CGI(void);
		void	initEnvp(const t_header& header);
		void	convertVector(void);
		void	executeScript(std::string file, std::string cgiContent);
};

#endif