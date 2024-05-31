/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   showUploads.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/13 13:23:18 by juvan-to          #+#    #+#             */
/*   Updated: 2024/05/31 14:39:42 by mgoedkoo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

// I think server stuff needs to be added here as well
std::string	getFolderContents(void)
{
	std::string		folderContents = "";
	std::string		line;
	DIR				*dr;
	struct dirent	*en;
	
	dr = opendir("./cgi-bin/uploads");
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
		{
			if (std::string(en->d_name) != "." && std::string(en->d_name) != "..")
			{
				line = "<div class='entry'><button class='delete-button' value='" + std::string(en->d_name) +"'onclick='deleteFile(this.value)'>Delete</button>";
				line += "<p>";
				line += std::string(en->d_name);
				line += "</p><br></div>";
				folderContents += line;
			}
		}
		if (folderContents == "")
			folderContents = "<p>No files uploaded</p>";
		closedir(dr);
	}
	else
		folderContents = "<p>No files uploaded</p>";
	return folderContents;
}

std::string	ServerManager::showUploads(const std::string &path, const std::string &status, const std::string &color)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	responseStream << fileStream.rdbuf();
	
	std::string response = "HTTP/1.1 " + color + status + RESET + "\r\n";
	std::string fileContents = responseStream.str();
	fileContents += getFolderContents();
	fileContents += "</ul></div></body></html>";
	
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return response;
}
