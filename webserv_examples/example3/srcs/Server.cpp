#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <vector>

Server::Server()
    : listen(80), serverName(""), root("/"), autoIndex(false), clientBodySize(0) {}

Server::~Server() {}

int Server::getListen() const
{
    return this->listen;
}

std::map<std::string, Location> Server::getLocations(void) const
{
    return this->locations;
}

std::string Server::getServerName(void) const
{
    return this->serverName;
}

std::string Server::getRoot(void) const
{
    return this->root;
}

std::map<std::vector<int>, std::string> Server::getErrorPage(void) const
{
    return this->errorPage;
}

std::map<std::string, int> Server::getRedirection(void) const
{
    return this->redirection;
}

bool Server::getAutoIndex(void) const
{
    return this->autoIndex;
}

unsigned int Server::getClientBodySize(void) const
{
    return this->clientBodySize;
}

std::string Server::getUploadPath() const
{
    return this->uploadedPath;
}

bool Server::fillServer(std::map<std::string, Location>& mapLocations, std::map<std::string, std::string>& mapSentence)
{
    std::map<std::string, std::string>::iterator it = mapSentence.begin();

    this->locations = mapLocations;
    while (it != mapSentence.end())
    {
        if (it->first == "listen")
            this->setListen(it->second);
        else if (it->first == "server_name")
            this->setServerName(it->second);
        else if (it->first == "root")
            this->setRoot(it->second);
        else if (it->first == "error_page")
        {
            if (this->setErrorPage(it->second))
                return true;
        }
        else if (it->first == "return")
        {
            if (this->setRedirection(it->second))
                return true;
        }
        else if (it->first == "autoindex")
            this->setAutoIndex(it->second);
        else if (it->first == "client_max_body_size")
            this->setClientBodySize(it->second);
        else if (it->first == "upload_path")
            this->setUploadPath(it->second);
        else
            return true;
        it++;
    }
    return false;
}

void Server::setListen(std::string& input)
{
    long parsedPort = strtod(input.c_str(), NULL);

    if (parsedPort < 0 || parsedPort > 65535)
        std::cout << "Warning : invalid port number" << std::endl;
    this->listen = parsedPort;
}

void Server::setServerName(std::string& input)
{
    this->serverName = input;
}

void Server::setRoot(std::string& input)
{
    this->root = input;
}

bool Server::setErrorPage(std::string const& sentence)
{
    std::stringstream sstream;
    std::vector<std::string> tokens;
    std::string token;
    std::vector<int> vecErrorCode;
    std::string errorPath;

    sstream << sentence;
    while (sstream >> token)
        tokens.push_back(token);

    if (tokens.size() < 2)
        return true;
    errorPath = tokens.back();
    tokens.pop_back();

    for (size_t i = 0; i < tokens.size(); i++)
        vecErrorCode.push_back(std::strtod(tokens[i].c_str(), NULL));
    this->errorPage.insert(std::pair<std::vector<int>, std::string>(vecErrorCode, errorPath));
    return false;
}

bool Server::setRedirection(std::string const& sentence)
{
    std::stringstream sstream;
    std::vector<std::string> tokens;
    std::string token;
    std::string redirectionPath;
    int redirectionCode;

    sstream << sentence;
    while (sstream >> token)
        tokens.push_back(token);

    if (tokens.size() != 2)
        return true;
    redirectionCode = std::strtod(tokens[0].c_str(), NULL);
    redirectionPath = tokens[1];
    this->redirection[redirectionPath] = redirectionCode;
    return false;
}

void Server::setAutoIndex(std::string& input)
{
    if (input == "on")
        this->autoIndex = true;
    else
        this->autoIndex = false;
}

void Server::setClientBodySize(std::string& input)
{
    this->clientBodySize = std::strtod(input.c_str(), NULL);
}

void Server::setUploadPath(std::string& input)
{
    this->uploadedPath = input;
}