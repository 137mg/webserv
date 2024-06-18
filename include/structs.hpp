/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   structs.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/06/13 16:46:08 by mgoedkoo      #+#    #+#                 */
/*   Updated: 2024/06/18 01:08:48 by Julia         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_HPP
#define STRUCTS_HPP

typedef struct s_header
{
	std::string	method;
	std::string	file;
	std::string	protocol;
	std::string	host;
	uint16_t	port;
	std::string	contentLength;
	std::string	contentType;
}	t_header;

typedef struct s_location
{
	bool						autoIndex;
	std::string					match;
	std::string					root;
	std::string					index;
	std::string					redirect;
	std::vector<std::string>	allowedMethods;
	std::vector<std::string>	cgiExtents;
	std::vector<std::string>	cgiPaths;
}	t_location;

typedef struct s_CGIProcess
{
    int stdinFd;
    int stdoutFd;
    int clientFd;
    pid_t pid;
}	t_CGIProcess;

#endif