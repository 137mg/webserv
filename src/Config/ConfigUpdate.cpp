/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigUpdate.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 12:36:34 by mirjam            #+#    #+#             */
/*   Updated: 2024/05/28 16:52:00 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

void	Config::updateServer(Server& server)
{
	uint16_t	u16;
	uint32_t	u32;
	
	if (_key == "listen" || _key == "client_max_body_size" || _key == "host")
	{
		if (_values.size() > 1)
			throw ConfigFileException();
		if (_key == "host")
		{
			server.host = _values[0];
			return;
		}
		u16 = stouint16(_values[0]);
		if (_key == "listen")
			server.port = u16;
		else
		{
			if (u16 > 10)
				throw ConfigFileException();
			u32 = MB * u16;
			server.clientMaxBodySize = u32;
		}
		return;
	}
	if (_key == "server_names")
	{
		updateVector(server.serverNames);
		return;
	}
	updateLocation(server.defaultLocation);
}

void	Config::updateErrorPages(Server& server)
{
	uint16_t	code;

	code = stouint16(_key);
	if (server.errorPages.count(code) == 0)
		throw ConfigFileException();
	if (_values.size() > 1)
		throw ConfigFileException();
	if (access(_values[0].c_str(), R_OK) == -1)
		throw ConfigFileException();
	server.errorPages[code] = _values[0];
}

void	Config::updateLocation(t_location& location)
{
	std::string	keys[] = {"auto_index", "match", "root", "index", "redirect",
	"allowed_methods", "cgi_extents", "cgi_paths"};
	size_t	i;

	for (i = 0; i < 8; i++)
	{
		if (_key == keys[i])
			break;
	}
	if (i < 5 && _values.size() > 1)
		throw ConfigFileException();
	switch (i)
	{
		case 0:
			if (_values[0] == "true")
				location.autoIndex = true;
			else if (_values[0] == "false")
				location.autoIndex = false;
			else
				throw ConfigFileException();
			break;
		case 1:
			location.match = _values[0];
			break;
		case 2:
			location.root = _values[0];
			break;
		case 3:
			location.index = _values[0];
			break;
		case 4:
			location.redirect = _values[0];
			break;
		case 5:
			updateVector(location.allowedMethods);
			break;
		case 6:
			updateVector(location.cgiExtents);
			break;
		case 7:
			updateVector(location.cgiPaths);
			break;
		default:
			throw ConfigFileException();
	}
}

void	Config::updateVector(std::vector<std::string>& vector)
{
	size_t	size;

	vector.clear();
	size = _values.size();
	for (size_t i = 0; i < size; i++)
		vector.push_back(_values[i]);
}
