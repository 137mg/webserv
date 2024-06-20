/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:52:49 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/20 14:04:43 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Manager.hpp"

class CGI
{
	private:
		char						**_envp;
		std::vector<std::string>	_envpVector;
		std::vector<std::string>	_outputBuffer;
		Server						_server;
		Manager				&_Manager;
	
	public:
		CGI(const Server& server, Manager &Manager);
		~CGI(void);
		void		initEnvp(const t_header& header);
		void		convertVector(void);
		void		executeScript(std::string file, std::string cgiRequest, int clientFd);
};

#endif