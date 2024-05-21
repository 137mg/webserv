#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <ctime>
#include <sys/stat.h>
#include <cstdio>
#include <dirent.h>
#include <errno.h>

#include "request.hpp"
#include "Cgi.hpp"

#ifdef __APPLE__
 #ifndef st_mtime
 #define st_mtime st_mtimespec.tv_sec
 #endif
#endif

#define N_OF_PAGES 3

class FileParser {
private:
    std::string rawFile;
    bool status;
public:
    FileParser(void);
    FileParser(const FileParser&);
    FileParser(const std::string&, bool);
    ~FileParser();

    std::string getRequestFile(void) const;
    int getRequestFileSize(void) const;
};

class Response
{
private:
    /* ------------------------- ATTRIBUTES ------------------------ */

    Request*	_req;		// Request object when the request is fully received, used to create response
    Location	_loc;

    StatusLine	_staLine;	// Fist line of http response
    std::string	_buffer;	// Buffer containing the response that will be send. Directly writing
    // headers into it.
    bool        _autoIndex;     // Sets to true if request is GET or HEAD, the target (after rooting) 
    // is a directory, and autoindex is on.
    bool		_endConn;
public:

    /* ------------------------ COPLIEN FORM ----------------------- */

    Response();
    Response(const Response& c);
    ~Response();
    Response& operator=(Response a);

		
    /* --------------------------- SETTERS ------------------------- */

    void setRequest(Request* req);
    void setStatusLine(const StatusLine& staLine);
    void setLocation(const Location&);


    /* --------------------------- GETTERS ------------------------- */
		
    const StatusLine&  getStatusLine() const;
    const std::string& getBuffer() const;

    int  getCode() const;
    bool getEndConn() const;
		
    static  std::string  get_filename_from_uri(const std::string&) ;
    /* --------------------------- METHODS ------------------------- */

    // Reset response object
    void clear();

    // Fill response buffer according to request object and status line previously set
    void fillBuffer(Request*, const Location&, const StatusLine&);


private:

    /* ----------------------- PRIVATE METHODS --------------------- */

    // Fills buffer with Content-length header
    void fillContentlengthHeader(const std::string& size);

    // Fills buffer with server header with server name (webserv)
    void fillServerHeader();

    // Fills buffer with Date header with the actual date
    void fillDateHeader();

    // Fills buffer with Last-Modified header (last modification of a file)
    void fillLastModifiedHeader(const char* uri);

    // Fills buffer with Location header (for HTTP redirection)
    void fillLocationHeader(const std::string& redirectedUri);

    void fillContentTypeHeader(void);
    void fillContentTypeHeader(const std::string&);

    // Fills buffer with status line
    void fillStatusLine(const StatusLine& staLine);

    // Transforms URI using index and root settings
    std::string reconstructFullURI(int method, std::string uri);

    // Replaces the location name that matched with root directive
    void replaceLocInUri(std::string& uri, const std::string& root);

    // Try to add all the indexs until one path is correct. If none are correct, throws a StatusLine
    // object with a 301 error code
    std::string addIndex(const std::string& uri, const std::vector<std::string>& indexs);
		
    // Compares the methods allowed from a location block with the requested method. If not allowed,
    // throws a StatusLine object with a 405 error code
    void checkMethods(int method, const std::vector<std::string>& methodsAllowed) const;

    // Fill the buffer with status line + 2 headers (server and date), then with the appropriate 
    // error page as body (by default or the one set in the server block from the config file)
    void fillError(const StatusLine& sta);

    // Execute CGI and sets the buffer with status line / headers / cgi output as payload
    void execCgi(const std::string& realUri, const cgi_pair& cgiConfig);
    void execGet(const std::string& realUri);
    void execPost(const std::string& realUri);
    void execDelete(const std::string& realUri);
    void execPut(const std::string&);

    std::vector<std::string> splitWithSep(const std::string&, char);

    void autoIndexDisplayer(const std::string&, std::string&);

    bool isResourceAFile(const std::string&) const;
    bool isMarkedForClosing(void) const;

    cgi_pair     getCgiExecutableName(const std::string&) const;

    std::string  getResourceExtension(const std::string&) const;
    std::string  getErrorPage(const StatusLine&);

    void setUp100Continue(void);
    bool client_expects_body_in_response(void) const;
public:
	
    /* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

    // Swap two Response object
    friend void swap(Response& a, Response& b);
	
};

void print_Loc(const Location&);

#endif
