#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <cctype>
#include <map>

#include "webserv.hpp"
#include "server.hpp"
#include "msg_format/StatusLine.hpp"
#include "msg_format/RequestLine.hpp"
#include "msg_format/Body.hpp"

#define HEADER_LIST_SIZE 41

#define REQ_LINE 0
#define HEADER 1
#define REQ_BODY 2
#define REQ_CHUNK_BODY 3
#define REQ_TAB_SIZE 4
#define READY 4

#define THROW_STATUS(extraInfo)	throw StatusLine(400, REASON_400, extraInfo)

class Request {
private:
    typedef bool (Request::*_req_options)(void);
    typedef std::pair<std::string, std::string> client_pair;

    /* información sobre el servidor que tramita la petición */
    server_vector _serv_v;      /* vector de servidores con mismo socket */
    client_pair   _client_info; /* información sobre el cliente (addr, port)*/
    int           _server_port; /* port del servidor */

    std::string _buffer;		// Store the request received
    std::string _line;
        

    
    size_t	_header_count;

    RequestLine	_request_line;	// Contains all the information from the request line
    header_map	_headers;	// Store the headers
    Body	_body;		// Store the body
	
    size_t	_stage;
	
    static const std::string _header_list[HEADER_LIST_SIZE];

public:
    Request(void);
    Request(const server_vector&, client_pair, int);
    Request(const Request& c);
    ~Request();
    Request& operator=(Request a);

    const std::string& get_client_addr(void) const;
    const std::string& get_client_port(void) const;
    std::string        get_server_host(void) const;
    int                get_server_port(void) const;
    const RequestLine& get_request_line(void) const;
    
    const header_map&          get_headers(void) const;
    header_map::const_iterator get_headers(const std::string&) const;
    
    const Body&        get_request_body(void) const;
    const std::string& get_body_string(void) const;
    int                get_body_size(void) const;
    
    int                get_method(void) const;
    const std::string& get_path(void) const;
    const std::string& get_virtual_path(void) const;
    const std::string& get_query(void) const;

    size_t             get_request_stage(void) const;

    const Location&    get_location(void) const;

    void set_path(const std::string&);

    void recv_buffer(const char[], int);

    void clear();
    void print() const;
	
private:
    /* Parseo de Request-Line */
    void parse_method(const std::string&);
    void parse_uri(std::string&);
    void parse_HTTP_version(const std::string&);
    bool parse_request_line(void);

    /* Parseo de Header-Line */
    void parse_header_requirements(void) const;
    void request_body_settings(void);
    bool parse_header_end(void);
    bool parse_header(void);

    /* Parseo de Request-Body */
    bool   transfer_encoding_is_chunked(void) const;
    bool   parse_chunk_req_size(void);
    bool   parse_chunk_req_chunk(void);
    bool   parse_chunk_req_body(void);
    bool   parse_request_body(void);

    /* Accesos a headers */
    std::string	  host(void) const;
    size_t        content_length(void) const;

    bool get_next_line(void);
  
    friend void swap(Request& a, Request& b);
};

#endif
