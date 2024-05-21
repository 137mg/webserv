#include "request.hpp"

Request::Request() :
    _serv_v(),
    _client_info(),
    _server_port(),
    _buffer(),
    _line(),
    _header_count(0),
    _request_line(),
    _headers(),
    _body(),
    _stage(REQ_LINE) { }

/* Constructor con información sobre la conexión de la petición */
Request::Request(const server_vector& serv_v, client_pair client_info, int server_port) :
    _serv_v(serv_v),
    _client_info(client_info),
    _server_port(server_port),
    _buffer(),
    _line(),
    _header_count(0),
    _request_line(),
    _headers(),
    _body(),
    _stage(REQ_LINE) { }

Request::Request(const Request& c) :
    _serv_v(c._serv_v),
    _client_info(c._client_info),
    _server_port(c._server_port),
    _buffer(c._buffer),
    _line(c._line),
    _header_count(c._header_count),
    _request_line(c._request_line),
    _headers(c._headers),
    _body(c._body),
    _stage(c._stage) { }

Request::~Request() { }

Request& Request::operator=(Request a) {
    if (this == &a) {
        return *this;
    }
    swap(*this, a);
    return *this;
}

const std::string& Request::get_client_addr(void) const {
    return _client_info.first;
}

const std::string& Request::get_client_port(void) const {
    return _client_info.second;
}

std::string Request::get_server_host(void) const {
    std::string host_name(host());
    size_t      port(host_name.find(':'));

    if (port != std::string::npos) {
	host_name = host_name.substr(0, port);
    }
    server_vector::const_iterator it = std::find_if(
	_serv_v.begin(),
	_serv_v.end(),
	find_server_by_host(host_name)
	);
    const Server* srv = (it == _serv_v.end()) ? _serv_v.front() : *it;

    return (srv->server_name.empty()) ? "" : host_name;
}

int Request::get_server_port(void) const {
    return _server_port;
}

const RequestLine& Request::get_request_line() const {
    return _request_line;
}
const header_map& Request::get_headers() const {
    return _headers;
}

header_map::const_iterator Request::get_headers(const std::string& header_name) const {
    return _headers.find(header_name);
}

const Body& Request::get_request_body() const {
    return _body;
}

const std::string& Request::get_body_string(void) const {
    return _body.get_body();
}

int Request::get_body_size(void) const {
    return _body.get_size();
}

int Request::get_method() const {
    return _request_line.get_method();
}

const std::string& Request::get_path() const {
    return _request_line.get_path();
}

const std::string& Request::get_virtual_path(void) const {
    return _request_line.get_virtual_path();
}

const std::string& Request::get_query() const {
    return _request_line.get_query();
}

/* get_location() trabaja con listas de servidors que comparten mismo puerto */
const Location& Request::get_location(void) const {
    std::string host_name(host());
    size_t      port_sep(host_name.find(':'));

    if (port_sep != std::string::npos) {
	host_name = host_name.substr(0, port_sep);
    }
    server_vector::const_iterator it = \
        std::find_if(_serv_v.begin(), _serv_v.end(), find_server_by_host(host_name));
    const Server* server_host = (it == _serv_v.end()) ? _serv_v.front() : *it;

    return server_host->get_location_by_path(get_path());
}

size_t Request::get_request_stage(void) const {
    return _stage;
}

void Request::set_path(const std::string& path) {
    _request_line.set_path(path);
}

void swap(Request& a, Request& b) {
    swap(a._buffer, b._buffer);
    std::swap(a._line, b._line);
    std::swap(a._serv_v, b._serv_v);
    std::swap(a._client_info, b._client_info);
    std::swap(a._server_port, b._server_port);
    std::swap(a._header_count, b._header_count);
    swap(a._request_line, b._request_line);
    swap(a._headers, b._headers);
    swap(a._body, b._body);
    std::swap(a._stage, b._stage);
}

void    Request::recv_buffer(const char new_buffer[], int size) {
    static _req_options req_table[REQ_TAB_SIZE] = {
        &Request::parse_request_line,
        &Request::parse_header,
        &Request::parse_request_body,
        &Request::parse_chunk_req_body
    };
    bool still_parsing = true;

    _buffer.append(new_buffer, size);
    if (_buffer.size() > REQ_MAX_SIZE) {
	throw(StatusLine(413, REASON_413, "Request: buffer exceeded max size"));
    }
    while (still_parsing == true) {
	still_parsing = (this->*req_table[_stage])();
    }
    if (_stage == READY) {
	throw StatusLine(200, REASON_200, "request received successfully: " + get_path());
    }
}

bool Request::parse_request_line(void) {
    enum request_token_f {method, uri, http_ver, size};
  
    if (!get_next_line()) {
	return false;
    }
    if (_line.empty()) {
	THROW_STATUS("Request-line: received empty request-line");
    }
    std::stringstream   line_ss(_line);
    std::string         token[size];

    for (int i = 0; i < 3; i++) {
	line_ss >> token[i];
    }
    if (line_ss.rdbuf()->in_avail()) {
	THROW_STATUS("Request-line: wrong number of tokens in request-line");
    }
    parse_method(token[method]);
    parse_uri(token[uri]);
    parse_HTTP_version(token[http_ver]);
    _stage = HEADER;
    return true;
}

void    Request::parse_method(const std::string& token) {
    std::string method_opts[NB_METHODS] = {"GET", "HEAD", "POST", "DELETE", "PUT"};

    for (int method_id = 0; method_id < NB_METHODS; method_id++) {
	if (!token.compare(method_opts[method_id])) {
	    _request_line.set_method(method_id);
	    return ;
	}
    }
    THROW_STATUS("unknown method in request");
}

void    Request::parse_uri(std::string& token) {
    if (token.size() > MAX_URI_LEN) {
	throw StatusLine(414, REASON_414, "Request-URI: exceeded size limit");
    }
    std::string allowed_ptcl[2] = { "http://", "https://" };
    size_t query_i = token.find('?');

    if (query_i != std::string::npos) {
	_request_line.set_query(token.substr(++query_i));
	token = token.substr(0, --query_i);
    }
    for (size_t i = 0; i < 2; i++) {
	if (!token.compare(0, allowed_ptcl[i].size(), allowed_ptcl[i])) {
	    std::string hostname = token.substr(0, token.find("/", allowed_ptcl[i].size()));
	    std::string abs_path = token.substr(hostname.size());

	    _headers.insert(std::pair<std::string, std::string>("Host", hostname));
	    if (abs_path.empty()) {
		abs_path.append("/");
	    }
	    _request_line.set_path(abs_path);
	    return ;
	}
    }
    if (token[0] == '/') {
	_request_line.parse_path(token, get_location().get_cgi_pass());
	return ;
    }
    THROW_STATUS("Request-URI: bad URI format");
}

void    Request::parse_HTTP_version(const std::string& token) {
    if (token.compare("HTTP/1.1")) {
        THROW_STATUS("HTTP-ver: unknown or not supported HTTP version");
    }
}

/* Comprueba :
 *   valor de Content-Length es num. mayor que cero
 *   header Host se encuentra presente
 *   valor en Expect es "100-continue"
 * Setea valores de Body si procede
 */ 
bool    Request::parse_header_end(void) {
    header_map::const_iterator cl = _headers.find("Content-Length");
    header_map::const_iterator hs = _headers.find("Host");
    header_map::const_iterator ex = _headers.find("Expect");

    if (hs == _headers.end()) {
        THROW_STATUS("Request-Header: no Host defined on request");
    }
    if (cl != _headers.end()) {
        char *ptr;

        int cl_field_value = strtol(cl->second.c_str(), &ptr, 0);
        if (*ptr || cl_field_value < 0) {
            THROW_STATUS("Request-Header: invalid Content-Length field value");
        }
    }
    if (_request_line.get_method() == PUT || _request_line.get_method() == POST ) {
        request_body_settings();
        _stage = (transfer_encoding_is_chunked() == true) ? REQ_CHUNK_BODY : REQ_BODY;
    } else {
        _stage = READY;
    }
    if (ex != _headers.end() && !ex->second.compare("100-continue")) {
        throw StatusLine(100, REASON_100, "");
    }
    return (_stage != READY);
}

bool    Request::parse_header(void) {
    if (!get_next_line()) {
        return false;
    }
    if (_line.empty()) {
        return parse_header_end();
    }
    if (_line.size() > MAX_HEADER_LEN) {
        THROW_STATUS("Request-Header: header line overflows");
    }
    std::string field_name, field_value;
    size_t      colon_i = _line.find(':'), value_i;

    if (colon_i == std::string::npos) {
        return true;
    }
    field_name = _line.substr(0, colon_i);
    value_i = _line.find_first_not_of(" ", ++colon_i);
    if (value_i == std::string::npos) {   // no field value
        return true;
    }
    field_value = _line.substr(value_i);
    if (_header_count++ > HEADER_LIMIT) {
        THROW_STATUS("Request-Header: too many headers on request");
    }
    for (size_t i = 0; i < HEADER_LIST_SIZE; i++) {
	if (!field_name.compare(_header_list[i])) {
	    _headers.insert(std::pair<std::string, std::string>(field_name, field_value));
	    break ;
	}
    }
    return true;
}

void    Request::request_body_settings(void) {
    find_server_by_host  comp(host());
    header_map::iterator cl = _headers.find("Content-Length");

    if (_serv_v.empty()) {
        throw StatusLine(500, REASON_500, "Request-Body: no server paired with socket");
    }
    _body.set_max_size(get_location().get_body_size());
    if (cl != _headers.end()) {
        _body.set_size(std::atoi(cl->second.c_str()));
    }
}

bool Request::parse_chunk_req_size(void) {
    //char*       last;

    if (_line.empty()) {
        THROW_STATUS("Request-Body: bad chunked-request syntax");
    }
    //int size = strtol(_line.c_str(), &last, 0);
    std::stringstream size_h_sstr;
    long size;

    size_h_sstr << std::hex << _line;
    size_h_sstr >> size;
    
    if ((size_h_sstr.rdbuf()->in_avail()) || size < 0) {
        THROW_STATUS("Request-Body: bad chunked-request syntax");
    }
    _body.set_size(static_cast<size_t>(size));
    _body.switch_stage();
    return true;
}

bool  Request::parse_chunk_req_chunk(void) {
    _body.recv_buffer_chunk(_line);
    _body.switch_stage();
    if (!_body.get_size()) {
	_stage = READY;
	return false;
    }
    return true;
}

bool Request::parse_chunk_req_body(void) {
    static _req_options req_chunk_body_opts[2] = {
	&Request::parse_chunk_req_size,
	&Request::parse_chunk_req_chunk
    };
    bool still_parsing = true;
    
    while ((still_parsing = get_next_line())) {
	still_parsing = (this->*req_chunk_body_opts[_body.stage])();
	if (!still_parsing) {
	    break ;
	}
    }
    return still_parsing;
}

bool    Request::parse_request_body(void) {
    get_next_line();
  
    _body.recv_buffer(_line);
    if (_body.get_body().size() == content_length()) {
	_stage = READY;
    }
    return false;
}

bool    Request::transfer_encoding_is_chunked(void) const {
    header_map::const_iterator te = _headers.find("Transfer-Encoding");
    header_map::const_iterator cl = _headers.find("Content-Length");

    if ((te != _headers.end() && !te->second.compare("chunked")) || (cl == _headers.end())) {
	return true;
    }
    return false;
}

std::string  Request::host(void) const {
    header_map::const_iterator h = _headers.find("Host");

    return (h != _headers.end()) ? h->second : "";
}

// Comportamiento indefinido si se llama antes de pasar por headerMeetsRequirements 
size_t  Request::content_length(void) const {
    header_map::const_iterator cl = _headers.find("Content-Length");

    if (cl != _headers.end()) {
        return std::atoi(cl->second.c_str());
    }
    return 0;
}

void Request::print(void) const {
    std::cout << "[ request ]\n";
    std::cout << "* -------------------- *\n\n";
    _request_line.print();
    for (header_map::const_iterator it = _headers.begin(); it != _headers.end(); it++) {
        std::cout << it->first << ": " << it->second << "\n";
    }
    std::cout << "\n" << _body.get_body() << "\n";
    std::cout << "* -------------------- *\n\n";
}

void Request::clear(void) {
    _buffer.clear();
    _line.clear();
    _header_count = 0;
    _request_line.clear();
    _headers.clear();
    _body.clear();
    _stage = REQ_LINE;
}

bool Request::get_next_line(void) {
    size_t crlf_i;
  
    _line.clear();
    if (_stage == REQ_BODY) {
	_line = _buffer;
	_buffer.clear();
	return true;
    }
    crlf_i = _buffer.find(CRLF);
    if (crlf_i == std::string::npos) {
	return false;
    }
    _line = _buffer.substr(0, crlf_i);
    _buffer = _buffer.substr(crlf_i + 2);
    return true;
}

const std::string Request::_header_list[HEADER_LIST_SIZE] = {
    "A-IM",             "Accept",          "Accept-Charset",                "Accept-Encoding",
    "Accept-Language",  "Accept-Datetime", "Access-Control-Request-Method", "Access-Control-Request-Headers",
    "Authorization",    "Cache-Control",   "Connection",                    "Content-Length",
    "Content-Type",     "Cookie",          "Date",                          "Expect",
    "Forwarded",        "From",            "Host",                          "If-Match",
    "If-Modified-Since","If-None-Match",   "If-Range",                      "If-Unmodified-Since",
    "Max-Forwards",     "Origin",          "Pragma",                        "Proxy-Authorization",
    "Range",            "Referer",         "TE",                            "Trailer",
    "Transfer-Encoding","User-Agent",      "Upgrade",                       "Via",
    "Warning",          "Sec-Fetch-User",  "Sec-Fetch-Site",                "Sec-Fetch-Mode",
    "Sec-Fetch-Dest"
};
