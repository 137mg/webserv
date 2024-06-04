/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerFiles.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/05/13 13:23:18 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/04 15:38:58 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// I think server variables need to be added here as well
static std::string	getFolderContents(void)
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
	return (folderContents);
}

std::string	Server::showUploads(std::string path, std::string status, std::string color)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	std::string			response;
	std::string			fileContents;

	responseStream << fileStream.rdbuf();
	fileContents = responseStream.str();
	fileContents += getFolderContents();
	fileContents += "</ul></div></body></html>";

	response = "HTTP/1.1 " + color + status + RESET + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return (response);
}

std::string	Server::serveFile(std::string path, std::string status)
{
	std::ifstream		fileStream(path);
	std::stringstream	responseStream;
	std::string			fileContents;
	std::string			response;

	responseStream << fileStream.rdbuf();
	fileContents = responseStream.str();

	response = "HTTP/1.1 " + status + "\r\n";
	response += "Content-Length: " + std::to_string(fileContents.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	if (path.find(".css") != std::string::npos)
		response += "Content-Type: text/css\r\n\r\n";
	else
		response += "Content-Type: text/html\r\n\r\n";
	response += fileContents;
	return (response);
}
