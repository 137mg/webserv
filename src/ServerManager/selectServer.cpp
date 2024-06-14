/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   selectServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 17:38:30 by juvan-to          #+#    #+#             */
/*   Updated: 2024/06/14 15:53:17 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

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

static t_header	parseRequest(std::string request)
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
	if (!portStr.empty())
		header.port = static_cast<uint16_t>(stoi(portStr));
	if (header.method == "POST")
	{
		header.contentLength = getValue(request, "Content-Length");
		header.contentType = getValue(request, "Content-Type");
	}
	return (header);
}

static bool	checkHeader(t_header header)
{
	if (header.method.empty() || header.file.empty() || header.protocol.empty())
		return (false);
	if (header.host.empty() || header.port == 0)
		return (false);
	if (header.method == "POST" && (header.contentLength.empty() || header.contentType.empty()))
		return (false);
	return (true);
}

void	ServerManager::selectServer(std::string buffer, int clientFd)
{
	t_header			header;
	uint16_t			port;
	std::vector<Server>	serverList;
	Server				server;
	size_t				i;
	size_t				j;

	header = parseRequest(buffer);
	if (!checkHeader(header))
	{
		buffer.clear();
		if (header.port == 0)
			header.port = _ports[0];
	}
	port = header.port;
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
