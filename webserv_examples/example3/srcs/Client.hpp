#pragma once

#include "Event.hpp"
#include "Location.hpp"
#include "Message/Response.hpp"
#include "Server.hpp"
#include "Http/HttpRequestManager.hpp"

enum RequestMessageParseState
{
    READY,
    METHOD,
    URI,
    HTTP_VERSION,
    HEADER,
    BODY,
    DONE,
    CHUNKED,
    ERROR,
};

class Client
{
private:
    int client_fd;
    Server* server;
    // event 등록;
	std::vector<unsigned char> readBuffer;
	RequestMessageParseState parseState;

	std::string strbodySize;
	size_t  longBodySize;
	bool haveToReadBody;

    void readMethod(const char* buffer, size_t readSize);
    void readUri(const char* buffer, size_t readSize);
    void readHttpVersion(const char* buffer, size_t readSize);
    void readHeader(const char* buffer, size_t readSize);
    void readBody(const char* buffer, size_t readSize);
    void readChunked(const char* buffer, size_t readSize);

public:
    Request request;
    Response response;
    Event* events;
    HttpRequestManager httpRequestManager;
	std::vector<unsigned char> sendBuffer;
    size_t writeIndex;
    typedef int PORT;
    typedef int SOCKET;
    Client();
    ~Client();

    void setFd(int fd);
    void setServer(Server* server);
    void setEvents(Event* event);

    Server* getServer(void) const;
    SOCKET getClientFd(void) const;

    void errorEventProcess(void);
    int readEventProcess(void);
    bool writeEventProcess(void);
    bool readMessage(void);
    bool checkMethod(void);
    bool isSendBufferEmpty(void);
};
