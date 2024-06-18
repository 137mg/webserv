/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:52:49 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/18 01:19:56 by Julia         ########   odam.nl         */
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
		ServerManager				&_serverManager;
	
	public:
		CGI(const Server& server, ServerManager &serverManager);
		~CGI(void);
		void		initEnvp(const t_header& header);
		void		convertVector(void);
		std::string	executeScript(std::string file, std::string cgiContent, int clientFd);
};

#endif