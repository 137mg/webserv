/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 14:52:49 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/16 15:39:14 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "ServerManager.hpp"
#include <sys/types.h>
#include <sys/wait.h>

class CGI
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
		void		executeScript(std::string cgiContent);
		char		**getEnvp(void) const;
};

#endif