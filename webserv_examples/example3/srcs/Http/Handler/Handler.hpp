#pragma once

#include "../../Message/Request.hpp"
#include "../../Color.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <fcntl.h>

# define SPACE " "
# define CRLF "\r\n"
# define SERVER_NAME "Master J&J Server"
# define SERVER_HTTP_VERSION "HTTP/1.1"
# define COLON ":"

class	Client;

std::vector<unsigned char>	BuildHeader(int status_code, std::map<std::string, std::string>& headers, bool include_crlf=true);
std::string	GetFileType(std::string file_name);
std::string	itos(int num);
std::vector<unsigned char>	stou(std::stringstream& ss);
std::vector<unsigned char>	BuildResponse(int status_code, std::map<std::string, std::string>& headers, std::vector<unsigned char>& body, bool is_static=true);
void						SetResponse(Client& client, int status_code, std::map<std::string, std::string>& headers, std::vector<unsigned char>& body);

void	ReadStaticFile(Client& client, std::string& file_name);
bool	IsDirectory(std::string path);
bool	IsRegularFile(std::string path);
bool	IsFileReadable(std::string path);
bool	IsFileExist(std::string path);
void	ServeStatic(Client& client, std::string& path);