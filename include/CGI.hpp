/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:52:49 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/07/02 01:41:33 by Julia         ########   odam.nl         */
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
		pid_t						_pid;
		int							_stdoutPipe[2];
		int							_stdinPipe[2];
		int							_clientFd;
		int							_status;
		Server						_server;
		Manager						&_Manager;
	
	public:
		std::string					errorResponse;
		CGI(const Server& server, Manager &Manager);
		~CGI(void);
		void		initEnvp(const t_header& header, std::string request);
		void		convertVector(void);
		void		executeScript(std::string CGIfile, std::string CGIdirectory, std::string cgiRequest, int clientFd);
		Server		&getServer(void);
		void		errorHandler(int errorCode, int clientFd);
		void		setNonBlocking(int fd);
		int			setUpPipes(void);
		int			setUpFork(void);
		int			getStatus(void);
};

#endif