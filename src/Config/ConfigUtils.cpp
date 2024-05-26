/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mirjam <mirjam@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 12:36:02 by mirjam            #+#    #+#             */
/*   Updated: 2024/05/26 00:16:53 by mirjam           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

static bool	isWhitespace(char c)
{
	if (std::isspace(c))
		return (true);
	return (false);
}

void	Config::removeWhitespace(void)
{
	std::string::iterator	it;
	std::string::iterator	ite;

	it = _line.begin();
	ite = _line.end();
	ite = remove_if(it, ite, isWhitespace);
	_line.resize(ite - it);
}

void	Config::parseLine(void)
{
	size_t		i;
	size_t		size;
	std::string	valueStr;

	_values.clear();
	i = _line.find('=');
	if (i == 0 || i == _line.size() - 1 || i == std::string::npos)
		throw ConfigFileException();
	_key = _line.substr(0, i);
	valueStr = _line.substr(i + 1);
	size = valueStr.size();
	if (valueStr[0] == '[' && valueStr[size - 1] == ']')
	{
		valueStr = valueStr.substr(1, size - 2);
		parseArray(valueStr);
	}
	else
		_values.push_back(valueStr);
	for (i = 0; i < _values.size(); i++)
	{
		size = _values[i].size();
		if (size == 0 || _values[i][0] != '\"' || _values[i].find('\"', 1) != size - 1)
			throw ConfigFileException();
		_values[i] = _values[i].substr(1, size - 2);
	}
}

void	Config::parseArray(const std::string& valueStr)
{
	size_t	i;
	size_t	j;

	i = 0;
	for (j = 0; valueStr[i]; i++)
	{
		if (valueStr[i] == '\"')
		{
			i = valueStr.find('\"', i + 1);
			if (i == std::string::npos)
				throw ConfigFileException();
			continue;
		}
		if (valueStr[i] == ',')
		{
			_values.push_back(valueStr.substr(j, i - j));
			j = i + 1;
		}
	}
	_values.push_back(valueStr.substr(j, i - j));
}

uint16_t	Config::stouint16(const std::string& str)
{
	uint16_t	u;
	int			tmp;

	tmp = stoi(str);
	if (tmp < 0 || tmp > 65535)
		throw ConfigFileException();
	u = static_cast<uint16_t>(tmp);
	return (u);
}
