#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <exception>
#include <string>
#include <sstream>
#include <map>
#include <string>
#include <unistd.h>

typedef std::map<std::string, std::string>  header_map;
typedef std::pair<std::string, std::string> cgi_pair;

enum methods { GET, HEAD, POST, DELETE, PUT };

// write loop buffer
#define S_BUFFR_WR 4000

// Number of methods implemented in webserv
#define NB_METHODS 5

// Max octets length of a status / request / header line
#define MAX_URI_LEN 8000
#define MAX_HEADER_LEN 8000
#define HEADER_LIMIT 100
#define REQ_MAX_SIZE 65536



// Number of octets that the request buffer can contain
#define REQUEST_BUFFER_SIZE 2046

// End the line for status / request line and headers
#define CRLF "\r\n"
#define CRLF_OCTET_SIZE 2

#define SEND_BUFFER_SIZE 65536

// The reason for each status line cose
#define REASON_100 "Continue"
#define REASON_101 "Switching Protocols"
#define REASON_200 "OK"
#define REASON_201 "Created"
#define REASON_202 "Accepted"
#define REASON_203 "Non-Authoritative Information"
#define REASON_204 "No Content"
#define REASON_205 "Reset Content"
#define REASON_206 "Partial Content"
#define REASON_300 "Multiple Choices"
#define REASON_301 "Moved Permanently"
#define REASON_302 "Found"
#define REASON_303 "See Other"
#define REASON_304 "Not Modified"
#define REASON_305 "Use Proxy"
#define REASON_307 "Temporary Redirect"
#define REASON_400 "Bad Request"
#define REASON_401 "Unauthorized"
#define REASON_402 "Payment Required"
#define REASON_403 "Forbidden"
#define REASON_404 "Not Found"
#define REASON_405 "Method Not Allowed"
#define REASON_406 "Not Acceptable"
#define REASON_407 "Proxy Authentication Required"
#define REASON_408 "Request Timeout"
#define REASON_409 "Conflict"
#define REASON_410 "Gone"
#define REASON_411 "Length Required"
#define REASON_412 "Precondition Failed"
#define REASON_413 "Payload Too Large"
#define REASON_414 "URI Too Long"
#define REASON_415 "Unsupported Media Type"
#define REASON_416 "Range Not Satisfiable"
#define REASON_417 "Expectation Failed"
#define REASON_426 "Upgrade Required"
#define REASON_431 "Request header fields too large"
#define REASON_500 "Internal Server Error"
#define REASON_501 "Not Implemented"
#define REASON_502 "Bad Gateway"
#define REASON_503 "Service Unavailable"
#define REASON_504 "Gateway Timeout"
#define REASON_505 "HTTP Version Not Supported"

// utils

template<class T>
std::string n_to_str(T nb) 
{
    std::string nbString;
    std::stringstream nbStream;
    nbStream << nb;
    nbString = nbStream.str();
    return nbString;
}

int ft_write(int, std::string, size_t);

#endif
