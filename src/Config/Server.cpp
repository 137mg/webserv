/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 17:13:40 by mgoedkoo          #+#    #+#             */
/*   Updated: 2024/05/17 16:03:27 by mirjam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{
	port = 0;
	clientMaxBodySize = MB;
	initErrorPages();
}

Server::~Server(void)
{
}

void	Server::initErrorPages(void)
{
	errorPages[400] = "";
	errorPages[401] = "";
	// status code 402 is not in use yet
	errorPages[403] = "";
	errorPages[404] = "html/PageNotFound.html";
	errorPages[405] = "";
	errorPages[406] = "";
	errorPages[407] = "";
	errorPages[408] = "";
	errorPages[409] = "";
	// status code 410 is for promotional purposes
	errorPages[411] = "";
	// status code 412 seems out of scope
	errorPages[413] = "";
	errorPages[414] = "";
	errorPages[415] = "";
	// status codes 416-429 are unnecessary I think
	errorPages[431] = "";
	// status code 451 deals with legal stuff, we are anarchists
	errorPages[500] = "";
	errorPages[501] = "";
	errorPages[502] = "";
	errorPages[503] = "";
	errorPages[504] = "";
	errorPages[505] = "";
	// status code 506 I don't even begin to understand
	errorPages[507] = "";
	errorPages[508] = "";
	// status code 510 seems out of scope too
	errorPages[511] = "";
}
