/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CGI.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: mgoedkoo <mgoedkoo@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/04/25 14:53:32 by juvan-to      #+#    #+#                 */
/*   Updated: 2024/06/12 16:27:42 by juvan-to      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(const Server& server) : _server(server)
{
	this->_envp = nullptr;
}

CGI::~CGI(void)
{
	if (this->_envp)
	{
		for (int i = 0; this->_envp[i]; i++)
			delete[] this->_envp[i];
		delete[] this->_envp;
	}
}

void	CGI::initEnvp(const t_header& header)
{
	std::string	fullContent = "CONTENT_TYPE=" + header.contentType;
	std::string	fullContentLen = "CONTENT_LENGTH=" + header.contentLength;
	std::string	fullAddress = "REMOTE_ADDR=" + _server.host;
	std::string	fullMethod = "REQUEST_METHOD=" + header.method;
	std::string	fullPort = "SERVER_PORT=" + std::to_string(_server.port);
	std::string	fullProtocol = "SERVER_PROTOCOL=" + header.protocol;
	
	this->_envpVector.push_back(fullContent);
	this->_envpVector.push_back(fullContentLen);
	this->_envpVector.push_back("HTTP_COOKIE=test");
	this->_envpVector.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_envpVector.push_back("HTTP_USER_AGENT=Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.1234.56 Safari/537.36");
	this->_envpVector.push_back("PATH_INFO=test");
	this->_envpVector.push_back("QUERY_STRING=");
	this->_envpVector.push_back(fullAddress);
	this->_envpVector.push_back("REMOTE_HOST=localhost");
	this->_envpVector.push_back(fullMethod);
	// this->_envpVector.push_back("SCRIPT_FILENAME=/cgi-bin/hello.py");
	// this->_envpVector.push_back("SCRIPT_NAME=hello.py");
	this->_envpVector.push_back("SERVER_NAME=Webserv");
	this->_envpVector.push_back(fullPort);
	this->_envpVector.push_back(fullProtocol);
	this->_envpVector.push_back("SERVER_SOFTWARE=webserv");
	this->_envpVector.push_back("REDIRECT_STATUS=200");
	this->_envpVector.push_back("");
}

void	CGI::convertVector(void)
{
	// Allocate memory for the char ** array
    this->_envp = new char*[this->_envpVector.size() + 1]; // +1 for the NULL terminator

    // Copy each string from the vector to the char ** array
    for (size_t i = 0; i < this->_envpVector.size(); ++i) {
        this->_envp[i] = new char[this->_envpVector[i].size() + 1];
        std::strcpy(this->_envp[i], this->_envpVector[i].c_str());
    }
    this->_envp[this->_envpVector.size()] = nullptr; // NULL terminator
}

void	buildHttpResponse(std::string content, int clientFd)
{
    std::string response = "";

	response = "HTTP/1.1 200 OK \r\n";
	response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
	response += "Connection: keep-alive\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += content;
	
	write(clientFd, response.c_str(), response.size());
	serverMessage(response, clientFd, GREEN);
	return;
	std::cout << content << clientFd << std::endl;
}

std::string	CGI::executeScript(std::string file, std::string cgiContent, int clientFd)
{
	int fds[2];
    std::string httpResponse = "";
    pid_t pid;
    char buffer[4096];
    ssize_t bytesRead;

    if (pipe(fds) == -1) {
        perror("pipe failed");
        return "";
    }

    pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return "";
    }

    if (pid == 0) {
    // 자식 프로세스
        close(fds[0]); // 읽기 끝 닫기

        // 표준 입력을 부모 프로세스에서 전달한 내용으로 리디렉션
        int pipeStdin[2];
        if (pipe(pipeStdin) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }

        pid_t innerPid = fork();
        if (innerPid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (innerPid == 0) { // 손자 프로세스
            close(pipeStdin[1]); // 쓰기 끝 닫기
            dup2(pipeStdin[0], STDIN_FILENO); // 표준 입력 리디렉션
            close(pipeStdin[0]); // 원본 파일 디스크립터 닫기

            dup2(fds[1], STDOUT_FILENO); // 표준 출력을 파이프로 리디렉션
            close(fds[1]); // 원본 파일 디스크립터 닫기

            const char *args[] = {file.c_str(), nullptr};
            execve(file.c_str(), const_cast<char **>(args), this->_envp);
            perror("execve failed");
            exit(EXIT_FAILURE);
        } else {
            close(pipeStdin[0]); // 읽기 끝 닫기
            write(pipeStdin[1], cgiContent.c_str(), cgiContent.size()); // CGI 콘텐츠 쓰기
            close(pipeStdin[1]); // 쓰기 끝 닫기
            waitpid(innerPid, nullptr, 0); // 손자 프로세스가 끝나기를 기다림
            exit(EXIT_SUCCESS);
        }
    } else {
        // 부모 프로세스
        close(fds[1]); // 쓰기 끝 닫기

        while ((bytesRead = read(fds[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            httpResponse += buffer;
        }

        close(fds[0]); // 읽기 끝 닫기
        waitpid(pid, nullptr, 0); // 자식 프로세스가 끝나기를 기다림
    }
	buildHttpResponse(httpResponse, clientFd);
	return (httpResponse);
	httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	write(clientFd, httpResponse.c_str(), httpResponse.size());
	serverMessage(httpResponse, clientFd, GREEN);
}
