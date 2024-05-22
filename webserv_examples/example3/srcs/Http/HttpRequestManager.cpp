#include "HttpRequestManager.hpp"
#include "../Client.hpp"
#include "../Location.hpp"
#include "Handler/Handler.hpp"
#include "Handler/RedirectHandler.hpp"

void	HttpRequestManager::Handle(Client& client)
{
	Parse(client);
	if (client.request.location.getRedirection().empty() == false)
	{
		HandleRedirect(client);
		return ;
	}
	if (client.request.is_static)
	{
		// Do Static
		HandleStatic(client);
	}
	else
	{
		// Do Dynamic
		if (!IsFileExist(client.request.path) || !IsRegularFile(client.request.path) || !IsFileReadable(client.request.path))
		{
			client.request.is_error = true;
			return HandleError(client, 500);
		}
		if (!OpenFd(client))
		{
			client.request.is_error = true;
			return HandleError(client, 500);
		}
		if (!RunCgi(client))
		{
			client.request.is_error = true;
			return HandleError(client, 500);
		}
		client.response.status_code = 200;
	}
}

void	HttpRequestManager::Parse(Client& client)
{
	Request&		request = client.request;
	bool			is_found = false;
	size_t			location_pos;
	std::string		found_uri;
	std::string		tmp_uri;
	std::map<std::string, Location> locations = client.getServer()->getLocations();
	std::map<std::string, Location>::iterator location;

	if (request.uri[request.uri.size() - 1] != '/')
		request.uri += "/";
	tmp_uri = request.uri;
	while (tmp_uri != "/")
	{
		if (is_found)
			break ;
		location_pos = tmp_uri.find_last_of('/');
		if (location_pos == std::string::npos)
			break;
		if (location_pos == 0)
			tmp_uri = "/";
		else
			tmp_uri = std::string(tmp_uri.begin(), tmp_uri.begin() + location_pos);
		for (location = locations.begin(); location != locations.end(); ++location)
		{
			if (tmp_uri == location->first)
			{
				found_uri = location->first;
				is_found = true;
				break;
			}
		}
	}
	if (is_found)
	{
		request.file_name = request.uri.substr(location_pos);
		request.file_name.erase(request.file_name.size() - 1);
	}
	else
		found_uri = "/";
	std::cout << "LOCATION: " << found_uri << '\n';
	request.cgi_path_info = "/";
	if (request.uri.find("cgi-bin") == std::string::npos)
	{
		if (request.method == "POST" && request.uri.find(".bla") != std::string::npos)
			request.is_static = false;
		else
			request.is_static = true;
	}
	else
	{
		request.is_static = false;
		size_t	pos = request.file_name.find('?');
		if (pos != std::string::npos)
		{
			request.cgi_args = request.file_name.substr(pos + 1);
			request.cgi_args = request.cgi_args.erase(request.cgi_args.size());
		}
		request.file_name = request.file_name.substr(0, pos);
		size_t	path_pos = request.file_name.find("/", 10);
		if (path_pos != std::string::npos)
		{
			request.cgi_path_info = request.file_name.substr(path_pos);
			request.file_name = request.file_name.substr(0, path_pos);
		}
	}
	request.location_uri = found_uri;
	request.location = client.getServer()->getLocations()[found_uri];
	request.path = request.location.getRoot() + request.file_name;
	if (client.getServer()->getUploadPath() != "")
	{
		request.cgi_path_info = request.location.getRoot();// + request.cgi_path_info;
		if (request.cgi_path_info[request.cgi_path_info.size() - 1] != '/')
			request.cgi_path_info += "/";
		request.cgi_path_info += client.getServer()->getUploadPath();
	}
	std::cout << "FILENAME: " << request.file_name << std::endl;
	std::cout << "PATH_INFO: " << request.cgi_path_info << std::endl;
	std::cout << "PATH: " << request.path << std::endl;
	std::cout << "METHOD: " << request.method << std::endl;

	std::map<std::string, std::string>::iterator it;
	for (it = request.headers.begin(); it != request.headers.end(); ++it)
	{
		std::cout << BOLDGREEN << it->first << " : " << it->second << RESET << '\n';
	}
};
