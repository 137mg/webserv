/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Post.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: Julia <Julia@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/23 23:05:36 by Julia         #+#    #+#                 */
/*   Updated: 2024/05/08 13:06:31 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CGI.hpp"
#include <sys/types.h>
#include <sys/wait.h>

void	Server::postRequest(std::string buffer, int bytesRead)
{
    CGI cgi;

    cgi.initEnvp(this->getHeader(buffer, "Content-Type"), this->getHeader(buffer, "Content-Length"));
    cgi.convertVector();
    cgi.executeScript(buffer, bytesRead);
    std::string response = serveFile("html/home.html", "200 OK", GREEN);
    write(this->_clientFd, response.c_str(), response.size());
    terminalMessage("Server response ", response);
}
