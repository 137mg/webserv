/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   selectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/11 17:44:51 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// need to decide what error to throw when needed variable is not found!
static std::string	getValue(std::string request, std::string key)
{
	std::string	value;
	size_t		startLine;
	size_t		startValue;
	size_t		endLine;
	
	startLine = request.find(key + ": ");
	if (startLine == std::string::npos)
		return ("");
	endLine = request.find("\r\n", startLine);
	if (endLine == std::string::npos)
		return ("");
	startValue = startLine + key.length() + 2;
	value = request.substr(startValue, endLine - startValue);
	return (value);
}

t_header	ServerManager::parseRequest(std::string request)
{
	std::istringstream	iss(request);
	t_header			header;
	std::string			portStr;
	size_t				i;

	std::getline(iss, header.method, ' ');
	std::getline(iss, header.file, ' ');
	std::getline(iss, header.protocol, '\r');
	header.host = getValue(request, "Host");
	i = header.host.find(":");
	portStr = header.host.substr(i + 1);
	header.host.erase(i);
	std::cout << header.host << std::endl;
	header.port = static_cast<uint16_t>(stoi(portStr));
	if (header.method == "POST")
	{
		header.contentLength = getValue(request, "Content-Length");
		header.contentType = getValue(request, "Content-Type");
	}
	return (header);
}

// still needs to handle server names!
void	ServerManager::selectServer(std::string buffer, int clientFd)
{
	t_header			header;
	uint16_t			port;
	std::vector<Server>	serverList;
	Server				server;

	header = parseRequest(buffer);
	port = header.port;
	serverList = _serverMap[port];
	server = serverList[0];
	server.handleRequest(header, buffer, clientFd);
}
