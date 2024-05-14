/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   postRequest.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/23 23:05:36 by Julia         #+#    #+#                 */
/*   Updated: 2024/05/14 15:57:21 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

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
