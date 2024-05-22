<<<<<<< HEAD
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trossel <trossel@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/26 16:47:18 by trossel           #+#    #+#             */
/*   Updated: 2022/11/03 17:42:55 by trossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Utils.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

ConfigParsor::ConfigParsor(const std::string &filename)
	: _filename(filename)
{
}

ConfigParsor::~ConfigParsor()
{
}

ConfigParsor::ConfigParsor(const ConfigParsor &rhs)
{
	*this = rhs;
}

ConfigParsor &ConfigParsor::operator=(const ConfigParsor &rhs)
{
	(void)rhs;
	return (*this);
}

const std::string &ConfigParsor::getFilename() const { return this->_filename;}

Location ConfigParsor::parseLocation(const JsonObject &locObject, const Location &defaultLoc) const
{
	Location loc;

	// Location path
	std::string location_path = locObject.getStringOrDefault("location_path", "");
	ft::trim(location_path);
	if (location_path.empty() && !defaultLoc.path.empty())
		throw std::logic_error("Location error: location_path cannot be empty");
	loc.path = location_path;

	loc.root_dir = locObject.getStringOrDefault("root", defaultLoc.root_dir);

	loc.auto_index = locObject.getBoolOrDefault("auto_index", defaultLoc.auto_index);

	loc.max_client_body_size = locObject.getIntOrDefault("max_client_body_size", defaultLoc.max_client_body_size);

	loc.upload_dir = locObject.getStringOrDefault("upload_dir", defaultLoc.upload_dir);

	// CGI_extensions
	std::vector<std::string> cgi_ext = locObject.getArrayOrEmpty("cgi_extensions").stringValues();
	if (cgi_ext.empty())
		cgi_ext = defaultLoc.cgi_extensions;
	for(std::vector<std::string>::iterator it = cgi_ext.begin();
		it != cgi_ext.end(); it++)
	{
		std::string ext = *it;
		ft::trim(ext);
		loc.cgi_extensions.push_back(ext);
	}

	// Indexes
	std::vector<std::string> indexes = locObject.getArrayOrEmpty("index").stringValues();
	if (indexes.empty())
		indexes = defaultLoc.indexes;
	for (std::vector<std::string>::iterator it = indexes.begin();
		it != indexes.end(); it++)
	{
		std::string index = *it;
		ft::trim(index);
		loc.indexes.push_back(index);
	}

	//error pages
	JsonObject errorsObj = locObject.getObjectOrEmpty("error_pages");
	std::map<std::string, JsonArray> err_map = errorsObj.getAllArrays();
	if (err_map.empty())
		loc.error_pages = defaultLoc.error_pages;
	else
	{
		for(std::map<std::string, JsonArray>::const_iterator it = err_map.begin();
				it != err_map.end(); it++)
		{
			std::vector<int> error_codes = it->second.intValues();
			for (size_t i(0); i < error_codes.size(); i++)
				loc.error_pages[error_codes[i]] = it->first;
		}
	}

	//redirections
	std::vector<JsonObject> redirObj = locObject.getArrayOrEmpty("redirections").ObjectValues();
	if (redirObj.empty())
		loc.redirections = defaultLoc.redirections;
	else
	{
		for(std::vector<JsonObject>::const_iterator it = redirObj.begin();
				it != redirObj.end(); it++)
		{
			std::string old_url = it->getString("old_url");
			Redirection redir;
			redir.status = it->getIntOrDefault("status", 302);
			redir.new_url = it->getStringOrDefault("new_url", "");
			ft::trim(old_url);
			ft::trim(redir.new_url);
			if (old_url[0] != '/')
				old_url = loc.path + '/' + old_url;
			if (!redir.new_url.empty() && redir.new_url[0] != '/')
				redir.new_url = loc.path + '/' + redir.new_url;
			loc.redirections[old_url] = redir;
		}
	}

	std::vector<std::string> disabled_requests = locObject.getArrayOrEmpty("disabled_methods").stringValues();
	for (size_t i(0); i < disabled_requests.size(); i++)
	{
		RequestType type = ft::RequestFromString(disabled_requests[i]);
		loc.requests_allowed.reset(type);
	}

	loc.cgi_bin = locObject.getStringOrDefault("cgi_bin", "");
	if (!loc.cgi_bin.empty())
		loc.isCGI = true;
	else
		loc.cgi_bin = defaultLoc.cgi_bin;
    char *tmp = getcwd(NULL, 0);
    if (tmp && !loc.cgi_bin.empty())
	{
		std::string cwd = tmp;
		loc.cgi_bin = (loc.cgi_bin[0] == '/') ? loc.cgi_bin : (cwd + '/' + loc.cgi_bin);
	}
	if (tmp)
		free(tmp);

	return loc;
}

Server ConfigParsor::parseServer(const JsonObject &serverObject) const
{
    Server serverCfg;

    serverCfg.addPort(serverObject.getIntOrDefault("port", 80));

	std::string address = serverObject.getStringOrDefault("address", "0.0.0.0");
	if (address == "localhost")
		address = "127.0.0.1";
	serverCfg.addAddress(address);

	std::vector<std::string> hosts = serverObject.getArrayOrEmpty("server_name").stringValues();
	for(std::vector<std::string>::iterator it = hosts.begin();
		it != hosts.end(); it++)
		serverCfg.addName(*it);

	// Default location
	Location defaultLocation = parseLocation(serverObject, Location());
	defaultLocation.path = "/";
	defaultLocation.isCGI = false;
	serverCfg.addLocation("/", defaultLocation);

	// Other locations
	std::vector<JsonObject> locations = serverObject.getArrayOrEmpty("locations").ObjectValues();
	for(std::vector<JsonObject>::iterator it = locations.begin();
		it != locations.end(); it++)
	{
		Location loc = parseLocation(*it, defaultLocation);
		serverCfg.addLocation(loc.path, loc);
	}
	return serverCfg;
}

Config ConfigParsor::parse() const
{
	Config cfg;
	JsonObject json;

	cfg.setValid(false);
	try
	{
		json.parseFromFile(this->_filename);
		cfg.setTimeout(json.getIntOrDefault("keepalive-timeout", 15));
		JsonArray serversArray = json.getArray("servers");
		std::vector<JsonObject> servers = serversArray.ObjectValues();

		std::vector<JsonObject>::const_iterator it;
		for(it = servers.begin(); it != servers.end(); it++)
			cfg.addServer(parseServer(*it));

		cfg.setValid(true);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
    std::cout << cfg << std::endl;
	return cfg;
}
=======
#include "../inc/ConfigParser.hpp"

ConfigParser::ConfigParser()
{
	this->_nb_server = 0;
}

ConfigParser::~ConfigParser() { }

/* printing parametrs of servers from config file */
int ConfigParser::print()
{
	std::cout << "------------- Config -------------" << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server #" << i + 1 << std::endl;
		std::cout << "Server name: " << _servers[i].getServerName() << std::endl;
		std::cout << "Host: " << _servers[i].getHost() << std::endl;
		std::cout << "Root: " << _servers[i].getRoot() << std::endl;
		std::cout << "Index: " << _servers[i].getIndex() << std::endl;
		std::cout << "Port: " << _servers[i].getPort() << std::endl;
		std::cout << "Max BSize: " << _servers[i].getClientMaxBodySize() << std::endl;
		std::cout << "Error pages: " << _servers[i].getErrorPages().size() << std::endl;
		std::map<short, std::string>::const_iterator it = _servers[i].getErrorPages().begin();
		while (it != _servers[i].getErrorPages().end())
		{
			std::cout << (*it).first << " - " << it->second << std::endl;
			++it;
		}
		std::cout << "Locations: " << _servers[i].getLocations().size() << std::endl;
		std::vector<Location>::const_iterator itl = _servers[i].getLocations().begin();
		while (itl != _servers[i].getLocations().end())
		{
			std::cout << "name location: " << itl->getPath() << std::endl;
			std::cout << "methods: " << itl->getPrintMethods() << std::endl;
			std::cout << "index: " << itl->getIndexLocation() << std::endl;
			if (itl->getCgiPath().empty())
			{
				std::cout << "root: " << itl->getRootLocation() << std::endl;
				if (!itl->getReturn().empty())
					std::cout << "return: " << itl->getReturn() << std::endl;
				if (!itl->getAlias().empty())
					std::cout << "alias: " << itl->getAlias() << std::endl;
			}
			else
			{
				std::cout << "cgi root: " << itl->getRootLocation() << std::endl;
				std::cout << "sgi_path: " << itl->getCgiPath().size() << std::endl;
				std::cout << "sgi_ext: " << itl->getCgiExtension().size() << std::endl;
			}
			++itl;
		}
		itl = _servers[i].getLocations().begin();
		std::cout << "-----------------------------" << std::endl;
	}
	return (0);
}

/* checking and read config file, split servers to strings and creating vector of servers */
int ConfigParser::createCluster(const std::string &config_file)
{
	std::string		content;
	ConfigFile		file(config_file);

	if (file.getTypePath(file.getPath()) != 1)
		throw ErrorException("File is invalid");
	if (file.checkFile(file.getPath(), 4) == -1)
		throw ErrorException("File is not accessible");
	content = file.readFile(config_file);
	if (content.empty())
		throw ErrorException("File is empty");
	removeComments(content);
	removeWhiteSpace(content);
	splitServers(content);
	if (this->_server_config.size() != this->_nb_server)
		throw ErrorException("Somthing with size"); //rewrite the sentence
	for (size_t i = 0; i < this->_nb_server; i++)
	{
		ServerConfig server;
		createServer(this->_server_config[i], server);
		this->_servers.push_back(server);
	}
	if (this->_nb_server > 1)
		checkServers();
	return (0);
}

/*remove comments from char # to \n */
void ConfigParser::removeComments(std::string &content)
{
	size_t pos;

	pos = content.find('#');
	while (pos != std::string::npos)
	{
		size_t pos_end;
		pos_end = content.find('\n', pos);
		content.erase(pos, pos_end - pos);
		pos = content.find('#');
	}
}

/* deleting whitespaces in the start, end and in the content if more than one */
void ConfigParser::removeWhiteSpace(std::string &content)
{
	size_t	i = 0;

	while (content[i] && isspace(content[i]))
		i++;
	content = content.substr(i);
	i = content.length() - 1;
	while (i > 0 && isspace(content[i]))
		i--;
	content = content.substr(0, i + 1);
}

/* spliting servers on separetly strings in vector */
void ConfigParser::splitServers(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	if (content.find("server", 0) == std::string::npos)
		throw ErrorException("Server did not find");
	while (start != end && start < content.length())
	{
		start = findStartServer(start, content);
		end = findEndServer(start, content);
		if (start == end)
			throw ErrorException("problem with scope");
		this->_server_config.push_back(content.substr(start, end - start + 1));
		this->_nb_server++;
		start = end + 1;
	}
}

/* finding a server begin and return the index of { start of server */
size_t ConfigParser::findStartServer (size_t start, std::string &content)
{
	size_t i;

	for (i = start; content[i]; i++)
	{
		if (content[i] == 's')
			break ;
		if (!isspace(content[i]))
			throw  ErrorException("Wrong character out of server scope{}");
	}
	if (!content[i])
		return (start);
	if (content.compare(i, 6, "server") != 0)
		throw ErrorException("Wrong character out of server scope{}");
	i += 6;
	while (content[i] && isspace(content[i]))
		i++;
	if (content[i] == '{')
		return (i);
	else
		throw  ErrorException("Wrong character out of server scope{}");

}

/* finding a server end and return the index of } end of server */
size_t ConfigParser::findEndServer (size_t start, std::string &content)
{
	size_t	i;
	size_t	scope;
	
	scope = 0;
	for (i = start + 1; content[i]; i++)
	{
		if (content[i] == '{')
			scope++;
		if (content[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return (start);
}

/* spliting line by separator */
std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	std::vector<std::string>	str;
	std::string::size_type		start, end;

	start = end = 0;
	while (1)
	{
		end = line.find_first_of(sep, start);
		if (end == std::string::npos)
			break;
		std::string tmp = line.substr(start, end - start);
		str.push_back(tmp);
		start = line.find_first_not_of(sep, end);
		if (start == std::string::npos)
			break;
	}
	return (str);
}

/* creating Server from string and fill the value */
void ConfigParser::createServer(std::string &config, ServerConfig &server)
{
	std::vector<std::string>	parametrs;
	std::vector<std::string>	error_codes;
	int		flag_loc = 1;
	bool	flag_autoindex = false;
	bool	flag_max_size = false;

	parametrs = splitParametrs(config += ' ', std::string(" \n\t"));
	if (parametrs.size() < 3)
		throw  ErrorException("Failed server validation");
	for (size_t i = 0; i < parametrs.size(); i++)
	{
		if (parametrs[i] == "listen" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getPort())
				throw  ErrorException("Port is duplicated");
			server.setPort(parametrs[++i]);
		}
		else if (parametrs[i] == "location" && (i + 1) < parametrs.size())
		{
			std::string	path;
			i++;
			if (parametrs[i] == "{" || parametrs[i] == "}")
				throw  ErrorException("Wrong character in server scope{}");
			path = parametrs[i];
			std::vector<std::string> codes;
			if (parametrs[++i] != "{")
				throw  ErrorException("Wrong character in server scope{}");
			i++;
			while (i < parametrs.size() && parametrs[i] != "}")
				codes.push_back(parametrs[i++]);
			server.setLocation(path, codes);
			if (i < parametrs.size() && parametrs[i] != "}")
				throw  ErrorException("Wrong character in server scope{}");
			flag_loc = 0;
		}
		else if (parametrs[i] == "host" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (server.getHost())
				throw  ErrorException("Host is duplicated");
			server.setHost(parametrs[++i]);
		}
		else if (parametrs[i] == "root" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getRoot().empty())
				throw  ErrorException("Root is duplicated");
			server.setRoot(parametrs[++i]);
		}
		else if (parametrs[i] == "error_page" && (i + 1) < parametrs.size() && flag_loc)
		{
			while (++i < parametrs.size())
			{
				error_codes.push_back(parametrs[i]);
				if (parametrs[i].find(';') != std::string::npos)
					break ;
				if (i + 1 >= parametrs.size())
					throw ErrorException("Wrong character out of server scope{}");
			}
		}
		else if (parametrs[i] == "client_max_body_size" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_max_size)
				throw  ErrorException("Client_max_body_size is duplicated");
			server.setClientMaxBodySize(parametrs[++i]);
			flag_max_size = true;
		}
		else if (parametrs[i] == "server_name" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getServerName().empty())
				throw  ErrorException("Server_name is duplicated");
			server.setServerName(parametrs[++i]);
		}
		else if (parametrs[i] == "index" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (!server.getIndex().empty())
				throw  ErrorException("Index is duplicated");
			server.setIndex(parametrs[++i]);
		}
		else if (parametrs[i] == "autoindex" && (i + 1) < parametrs.size() && flag_loc)
		{
			if (flag_autoindex)
				throw ErrorException("Autoindex of server is duplicated");
			server.setAutoindex(parametrs[++i]);
			flag_autoindex = true;
		}
		else if (parametrs[i] != "}" && parametrs[i] != "{")
		{
			if (!flag_loc)
				throw  ErrorException("Parametrs after location");
			else
				throw  ErrorException("Unsupported directive");
		}
	}
	if (server.getRoot().empty())
		server.setRoot("/;");
	if (server.getHost() == 0)
		server.setHost("localhost;");
	if (server.getIndex().empty())
		server.setIndex("index.html;");
	if (ConfigFile::isFileExistAndReadable(server.getRoot(), server.getIndex()))
		throw ErrorException("Index from config file not found or unreadable");
	if (server.checkLocaitons())
		throw ErrorException("Locaition is duplicated");
	if (!server.getPort())
		throw ErrorException("Port not found");
	server.setErrorPages(error_codes);
	if (!server.isValidErrorPages())
		throw ErrorException("Incorrect path for error page or number of error");
}

/* comparing strings from position */
int	ConfigParser::stringCompare(std::string str1, std::string str2, size_t pos)
{
	size_t	i;

	i = 0;
	while (pos < str1.length() && i < str2.length() && str1[pos] == str2[i])
	{
		pos++;
		i++;
	}
	if (i == str2.length() && pos <= str1.length() && (str1.length() == pos || isspace(str1[pos])))
		return (0);
	return (1);
}

/* checking repeat and mandatory parametrs*/
void ConfigParser::checkServers()
{
	std::vector<ServerConfig>::iterator it1;
	std::vector<ServerConfig>::iterator it2;

	for (it1 = this->_servers.begin(); it1 != this->_servers.end() - 1; it1++)
	{
		for (it2 = it1 + 1; it2 != this->_servers.end(); it2++)
		{
			if (it1->getPort() == it2->getPort() && it1->getHost() == it2->getHost() && it1->getServerName() == it2->getServerName())
				throw ErrorException("Failed server validation");
		}
	}
}

std::vector<ServerConfig>	ConfigParser::getServers()
{
	return (this->_servers);
}
>>>>>>> 8b3d215 (add examples)
