/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SelectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/21 17:51:44 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Manager.hpp"

std::string	Manager::getValue(std::string request, std::string key)
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

t_header	Manager::parseHeader(std::string request, uint16_t port)
{
	std::istringstream	iss(request);
	t_header			header;
	size_t				i;

	std::getline(iss, header.method, ' ');
	std::getline(iss, header.file, ' ');
	std::getline(iss, header.protocol, '\r');
	header.host = getValue(request, "Host");
	i = header.host.find(":");
	if (i != std::string::npos)
		header.host.erase(i);
	header.port = port;
	if (header.method == "POST")
	{
		header.contentLength = getValue(request, "Content-Length");
		header.contentType = getValue(request, "Content-Type");
	}
	return (header);
}

uint16_t	Manager::findPort(int clientFd)
{
	int		listenFd;
	size_t	i;
	size_t	size;

	listenFd = _fdMap[clientFd];
	size = _listenFds.size();
	for (i = 0; i < size; i++)
	{
		if (listenFd == _listenFds[i])
			break;
	}
	return (_ports[i]);
}

void	Manager::selectServer(std::string buffer, int clientFd)
{
	t_header			header;
	uint16_t			port;
	std::vector<Server>	serverList;
	Server				server;
	size_t				i;
	size_t				j;

	port = findPort(clientFd);
	if (buffer.empty())
	{
		server = _serverMap[port][0];
		server.handleRequest(header, buffer, clientFd);
		return;
	}
	header = parseHeader(buffer, port);
	serverList = _serverMap[port];
	for (i = 0; i < serverList.size(); i++)
	{
		for (j = 0; j < serverList[i].serverNames.size(); j++)
		{
			if (header.host == serverList[i].serverNames[j])
			{
				server = serverList[i];
				break;
			}
		}
		if (j != serverList[i].serverNames.size())
			break;
	}
	if (i == serverList.size())
		server = serverList[0];
	server.handleRequest(header, buffer, clientFd);
}
