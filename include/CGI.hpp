/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: juvan-to <juvan-to@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:52:49 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/04/30 00:00:07 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "Server.hpp"
#include <sys/types.h>
#include <sys/wait.h>

class CGI : public Server
{
	private:
		char						**_envp;
		std::vector<std::string>	_envpVector;
	
	public:
		CGI();
		~CGI();
		CGI(const CGI &other);
		CGI & operator=(const CGI &other);

		void		initEnvp(std::string content, std::string contentLength);
		void		convertVector(void);
		void		executeScript(std::string cgiContent, int bytesRead);
		char		**getEnvp(void) const;

		
};

#endif