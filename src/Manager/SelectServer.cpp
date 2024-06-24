/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SelectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/24 15:59:37 by mgoedkoo         ###   ########.fr       */
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

t_header	Manager::parseHeader(std::string request, int clientFd)
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
	header.port = findPort(clientFd);
	if (getValue(request, "Transfer-Encoding") == "chunked")
	{
		i = request.size() - (request.find("\r\n\r\n") + 4);
		header.contentLength = std::to_string(i);
	}
	else
		header.contentLength = getValue(request, "Content-Length");
	header.contentType = getValue(request, "Content-Type");
	return (header);
}

void	Manager::selectServer(std::string buffer, int clientFd)
{
	t_header			header;
	std::vector<Server>	serverList;
	Server				server;
	size_t				i;
	size_t				j;

	header = parseHeader(buffer, clientFd);
	serverList = _serverMap[header.port];
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
