<<<<<<< HEAD
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trossel <trossel@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/21 11:11:21 by trossel           #+#    #+#             */
/*   Updated: 2022/10/30 17:26:12 by trossel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() :
		max_client_body_size(0),
		isCGI(false),
		auto_index(false),
		path(""),
		root_dir(""),
		cgi_bin(""),
		upload_dir("")
{
	requests_allowed.set();
}

Location::Location(const Location &rhs)
{
	*this = rhs;
=======
#include "../inc/Location.hpp"

Location::Location()
{
	this->_path = "";
	this->_root = "";
	this->_autoindex = false;
	this->_index = "";
	this->_return = "";
	this->_alias = "";
	this->_client_max_body_size = MAX_CONTENT_LENGTH;
	this->_methods.reserve(5);
	this->_methods.push_back(1);
	this->_methods.push_back(0);
	this->_methods.push_back(0);
	this->_methods.push_back(0);
	this->_methods.push_back(0);
}

Location::Location(const Location &other)
{
	this->_path = other._path;
	this->_root = other._root;
	this->_autoindex = other._autoindex;
	this->_index = other._index;
	this->_cgi_path = other._cgi_path;
	this->_cgi_ext = other._cgi_ext;
	this->_return = other._return;
	this->_alias = other._alias;
    this->_methods = other._methods;
	this->_ext_path = other._ext_path;
	this->_client_max_body_size = other._client_max_body_size;
>>>>>>> 8b3d215 (add examples)
}

Location &Location::operator=(const Location &rhs)
{
<<<<<<< HEAD
	if (&rhs == this)
		return *this;
	max_client_body_size = rhs.max_client_body_size;
	requests_allowed = rhs.requests_allowed;
	isCGI = rhs.isCGI;
	indexes = rhs.indexes;
	auto_index = rhs.auto_index;
	path = rhs.path;
	upload_dir = rhs.upload_dir;
	root_dir = rhs.root_dir;
	cgi_bin = rhs.cgi_bin;
	error_pages = rhs.error_pages;
	cgi_extensions = rhs.cgi_extensions;
	redirections = rhs.redirections;
	return *this;
}

Location::~Location()
{
}

const Redirection *	 Location::findRedirection(const std::string &req_path) const
{
	std::map<std::string, Redirection>::const_iterator it = redirections.find(req_path);
	if (it != redirections.end())
		return &(it->second);
	return NULL;
}

std::ostream &operator<<(std::ostream &os, const Location &loc)
{
	std::string method_names[] = {"GET", "POST", "DELETE", "PUT", "HEAD", "PATCH"};

	os << YELLOW << "\ttype: " << RESET;
	if (loc.isCGI)
		os << "CGI PROXY";
	else
	{
		os << "STANDART";
		os << YELLOW << "\n\troot_dir: " << RESET << loc.root_dir;
	}

	os << YELLOW << "\n\tmax_client_body_size: " << RESET << loc.max_client_body_size;

	if (!loc.indexes.empty())
	{
		os << YELLOW << "\n\tindexes: " << RESET;
		for (size_t j = 0; j < loc.indexes.size(); j++)
			os << loc.indexes[j] << " ";
	}

	if (!loc.error_pages.empty())
	{
		os << YELLOW << "\n\terror pages: " << RESET << std::endl;
		for (std::map<int, std::string>::const_iterator it = loc.error_pages.begin();
				it != loc.error_pages.end(); it++)
			os << "\t\t" <<  it->first << ": " << it->second << std::endl;
	}

	if (!loc.redirections.empty())
	{
		os << YELLOW << "\tredirections: " << RESET << std::endl;
		for (std::map<std::string, Redirection>::const_iterator it = loc.redirections.begin();
				it != loc.redirections.end(); it++)
		{
			os << "\t\t" <<  it->first << " --> ";
			if (it->second.new_url.empty())
				os << it->second.status << std::endl;
			else if (it->second.status == 0)
				os << it->second.new_url << " (302)" << std::endl;
			else
				os << it->second.new_url << " (" << it->second.status << ")" << std::endl;
		}
	}

	os << YELLOW << "\tcgi_bin: " << RESET << loc.cgi_bin;
	os << YELLOW << "\n\tcgi_extensions: " << RESET;
	if (!loc.cgi_extensions.empty())
	{
		for (size_t j = 0; j < loc.cgi_extensions.size(); j++)
			os << loc.cgi_extensions[j] << " ";
	}
	else
		os << "none";

	os << YELLOW << "\n\tauto_index: " << RESET << (loc.auto_index ? "true" : "false");

	os << YELLOW << "\n\tRequests disabled: " << RESET;
	if (loc.requests_allowed.all())
		os << "none";
	else
	{
		for (unsigned int i(0); i < Unknown; i++)
		{
			if (!loc.requests_allowed[i])
				os << method_names[i] << " ";
		}
	}
	os << std::endl << std::endl;
	return os;
}
=======
	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_root = rhs._root;
		this->_autoindex = rhs._autoindex;
		this->_index = rhs._index;
		this->_cgi_path = rhs._cgi_path;
		this->_cgi_ext = rhs._cgi_ext;
		this->_return = rhs._return;
		this->_alias = rhs._alias;
		this->_methods = rhs._methods;
		this->_ext_path = rhs._ext_path;
		this->_client_max_body_size = rhs._client_max_body_size;
    }
	return (*this);
}

Location::~Location() { }

/* set functions */
void Location::setPath(std::string parametr)
{
	this->_path = parametr;
}

void Location::setRootLocation(std::string parametr)
{
	if (ConfigFile::getTypePath(parametr) != 2)
		throw ServerConfig::ErrorException("root of location");
	this->_root = parametr;
}

void Location::setMethods(std::vector<std::string> methods)
{
	this->_methods[0] = 0;
	this->_methods[1] = 0;
	this->_methods[2] = 0;
	this->_methods[3] = 0;
	this->_methods[4] = 0;

	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == "GET")
			this->_methods[0] = 1;
		else if (methods[i] == "POST")
			this->_methods[1] = 1;
		else if (methods[i] == "DELETE")
			this->_methods[2] = 1;
		else if (methods[i] == "PUT")
			this->_methods[3] = 1;
		else if (methods[i] == "HEAD")
			this->_methods[4] = 1;
		else
			throw ServerConfig::ErrorException("Allow method not supported " + methods[i]);
	}
}

void Location::setAutoindex(std::string parametr)
{
	if (parametr == "on" || parametr == "off")
		this->_autoindex = (parametr == "on");
	else
		throw ServerConfig::ErrorException("Wrong autoindex");
}

void Location::setIndexLocation(std::string parametr)
{
	this->_index = parametr;
}

void Location::setReturn(std::string parametr)
{
	this->_return = parametr;
}

void Location::setAlias(std::string parametr)
{
	this->_alias = parametr;
}

void Location::setCgiPath(std::vector<std::string> path)
{
	this->_cgi_path = path;
}

void Location::setCgiExtension(std::vector<std::string> extension)
{
	this->_cgi_ext = extension;
}

void Location::setMaxBodySize(std::string parametr)
{
	unsigned long body_size = 0;

	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (parametr[i] < '0' || parametr[i] > '9')
			throw ServerConfig::ErrorException("Wrong syntax: client_max_body_size");
	}
	if (!ft_stoi(parametr))
		throw ServerConfig::ErrorException("Wrong syntax: client_max_body_size");
	body_size = ft_stoi(parametr);
	this->_client_max_body_size = body_size;
}

void Location::setMaxBodySize(unsigned long parametr)
{
	this->_client_max_body_size = parametr;
}

/* get functions */
const std::string &Location::getPath() const
{
	return (this->_path);
}

const std::string &Location::getRootLocation() const
{
	return (this->_root);
}

const std::string &Location::getIndexLocation() const
{
	return (this->_index);
}

const std::vector<short> &Location::getMethods() const
{
	return (this->_methods);
}

const std::vector<std::string> &Location::getCgiPath() const
{
	return (this->_cgi_path);
}

const std::vector<std::string> &Location::getCgiExtension() const
{
	return (this->_cgi_ext);
}

const bool &Location::getAutoindex() const
{
	return (this->_autoindex);
}

const std::string &Location::getReturn() const
{
	return (this->_return);
}

const std::string &Location::getAlias() const
{
	return (this->_alias);
}

const std::map<std::string, std::string> &Location::getExtensionPath() const
{
	return (this->_ext_path);
}

const unsigned long &Location::getMaxBodySize() const
{
	return (this->_client_max_body_size);
}

/* for printing allowed methods*/
std::string Location::getPrintMethods() const
{
	std::string res;
	if (_methods[4])
		res.insert(0, "HEAD");
	if (_methods[3])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "PUT");
	}
	if (_methods[2])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "DELETE");
	}
	if (_methods[1])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "POST");
	}
	if (_methods[0])
	{
		if (!res.empty())
			res.insert(0, ", ");
		res.insert(0, "GET");
	}
	return (res);
}
>>>>>>> 8b3d215 (add examples)
