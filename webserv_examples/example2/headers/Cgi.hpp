#ifndef CGI_HPP
# define CGI_HPP

#include "webserv.hpp"
#include "request.hpp"
#include "response.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <sys/wait.h>
#include <errno.h>

#define N_ENV_HEADER     16
#define N_ENV_VAR        36
#define S_BUFFR_CGI_PIPE 4000
#define S_BUFFR_PWD      4095

#define EXECVE_FAIL EXIT_FAILURE 

class CGI
{
private:
    char **_envvar;
    char **_args;

    Request *_req;

    int _fdIN[2];
    int _fdOut[2];

    std::string _resource_path;
    std::string _document_root;
    std::string _cgi_path;
    std::string _path_info;
    std::string _host;
    
    std::string _raw_response;

    StatusLine _status_line;
    std::map<std::string, std::string> _header_map;
    std::string _body_string;

		
public:

    /* ------------------------ COPLIEN FORM ----------------------- */

    CGI(Request *, const std::string &, const cgi_pair &);
    CGI &operator=(CGI &);
    ~CGI();
	
    /* ------------------------ PUBLIC METHODS ----------------------- */

    void executeCGI();
    void parse_response(void);

    std::string getHeaders(void) const;
    std::string getBody(void) const;

    const StatusLine& getStatusLine(void) const;

    bool isHeaderDefined(const std::string&) const;
private:
	

    /* ------------------------ PRIVATE METHODS ----------------------- */
    void close_fdIN(void);
    void close_fdOut(void);

    void set_env_variables(void);
    void set_args(void);

    void executeCGI_read(void);
    
    void parse_normalize(void);
    void parse_response_headers(const std::string&);
    void parse_response_body(const std::string&);
    void parse_status_line(void);

    std::string set_host(void);
    std::string set_document_root(const Location&, const std::string&);
    std::string set_resource_path(const std::string&, const std::string&);
    std::string set_cgi_path(const std::string&);
    std::string set_path_info(void);
    
    void mySwap(CGI &, CGI &);
    
    CGI(void);
    CGI(CGI const &);

};

#endif
