#include "Cgi.hpp"

std::string CGI::set_host(void) {
    return _req->get_server_host();
}

/* 
 * setea el directorio raíz donde trabaja el CGI:
 * comprueba si root está configurado como absoluto 
 */
std::string CGI::set_document_root(const Location& loc, const std::string& cwd) {
    return (!loc.get_root().empty() && loc.get_root().at(0) != '/') ?
	cwd + "/" + loc.get_root() :
	loc.get_root();
}

std::string CGI::set_resource_path(const std::string& uri, const std::string& cwd) {
    /* comprueba si la ruta al recurso tiene una raíz absoluta */
    if (!uri.empty() && uri.at(0) == '/') {
	return uri;
    }
    /* monta ruta relativa */
    std::string abs_path(std::string(cwd) + "/" + uri);
    return abs_path;
}

std::string CGI::set_cgi_path(const std::string& cgi_path)
{
    std::string abs_path;

    /* comprueba si la ruta al CGI ya está configurada como absoluta */
    if (!cgi_path.empty() && cgi_path.at(0) == '/') {
	return cgi_path;
    }
    /* ruta relativa: construimos con la raíz */
    abs_path.append(_document_root);
    if (abs_path.at(abs_path.size() - 1) != '/') {
	abs_path.push_back('/');
    }
    /* añade ruta relativa a CGI */
    abs_path.append(cgi_path);
    return abs_path;
}

void CGI::close_fdIN(void) {
    if (_fdIN[0] != -1) {
        if (close(_fdIN[0]) == -1) {
	    std::cerr << "[CGI] " << strerror(errno) << "\n";
	}
        _fdIN[0] = -1;
    }
    if (_fdIN[1] != -1) {
        if (close(_fdIN[1]) == -1) {
	    std::cerr << "[CGI] " << strerror(errno) << "\n";
	}
        _fdIN[1] = -1;
    }
}

void CGI::close_fdOut(void) {
    if (_fdOut[0] != -1) {
        if (close(_fdOut[0]) == -1) {
	    std::cerr << "[CGI] " << strerror(errno) << "\n";
	}
        _fdOut[0] = -1;
    }
    if (_fdOut[1] != -1) {
	if (close(_fdOut[1]) == -1) {
	    std::cerr << "[CGI] " << strerror(errno) << "\n";
	}
        _fdOut[1] = -1;
    }
}

/*
 *  !! CHECK !!
 *  for (int j = 0; j < N_ENV_VAR && _envvar[j] != NULL; j++) { 
 *     std::cerr << _envvar[i] << "\n";
 *   }
 *
 */

void CGI::set_env_variables(void) {
    std::string request_uri = (_req->get_query().empty()) ?
	_req->get_path() :
	_req->get_path() + "?" + _req->get_query();
    std::string request_method = (_req->get_method() == GET) ? "GET" : "POST";
    std::string _header_env[N_ENV_HEADER] = {
	"Content-Type",   "Host",            "Accept",
	"Accept-Charset", "Accept-Encoding", "Accept-Language",
	"Connection",     "User-Agent",      "Cookie",
	"Sec-Fetch-User", "Sec-Fetch-Site",  "Sec-Fetch-Dest",
	"Sec-Fetch-Mode", "Cache-Control",   "Pragma",
	"Referer"
    };
    std::string _var_env[N_ENV_HEADER] = {
	"CONTENT_TYPE",        "HTTP_HOST",            "HTTP_ACCEPT",
	"HTTP_ACCEPT_CHARSET", "HTTP_ACCEPT_ENCODING", "HTTP_ACCEPT_LANGUAGE",
	"HTTP_CONNECTION",     "HTTP_USER_AGENT",      "HTTP_COOKIE",
	"HTTP_SEC_FETCH_USER", "HTTP_SEC_FETCH_SIZE",  "HTTP_SEC_FETCH_DEST",
	"HTTP_SEC_FECTH_MODE", "HTTP_CACHE_CONTROL",   "HTTP_PRAGMA",
	"HTTP_REFERER"
    };
    header_map::const_iterator header_request;
    size_t i = 0;
    
    for (size_t cont = 0; cont < N_ENV_HEADER; cont++) {
	header_request = _req->get_headers(_header_env[cont]);
	if (header_request != _req->get_headers().end()) {
	    _envvar[i++] = strdup((_var_env[cont] + "=" + header_request->second).c_str());
	}
    }
    header_map::const_iterator header_auth = _req->get_headers("Authorization");
    std::string auth_field_val;
    if (header_auth != _req->get_headers().end()) {
	auth_field_val = header_auth->second;
    }
    _envvar[i++] = strdup(std::string("AUTH_TYPE=" + auth_field_val).c_str());
    if (_req->get_method() == POST && !_req->get_body_string().empty()) {
	std::stringstream content_length;

	content_length << _req->get_body_size();
	_envvar[i++] = strdup(std::string("CONTENT_LENGTH=" + content_length.str()).c_str());
    }
    if (_req->get_method() == GET && !_req->get_query().empty()) {
	_envvar[i++] = strdup(std::string("QUERY_STRING=" + _req->get_query()).c_str());
	_envvar[i++] = strdup("CONTENT_TYPE=text/html");
    } else {
	_envvar[i++] = strdup("QUERY_STRING=");
    }
    _envvar[i++] = strdup(std::string("DOCUMENT_ROOT=" + _document_root).c_str());
    _envvar[i++] = strdup("GATEWAY_INTERFACE=CGI/1.1");
    _envvar[i++] = strdup(std::string("PATH_INFO=" + _req->get_virtual_path()).c_str());
    if (!_req->get_virtual_path().empty()) {
	std::string           pt_field_value(_document_root);
	std::string::iterator it = --pt_field_value.end();

	if (*it == '/') {
	    pt_field_value.erase(it);
	}
	pt_field_value.append(_req->get_virtual_path());
	_envvar[i++] = strdup(std::string("PATH_TRANSLATED=" + pt_field_value).c_str());
    }
    _envvar[i++] = strdup("REDIRECT_STATUS=200");
    _envvar[i++] = strdup(std::string("REMOTE_ADDR=" + _req->get_client_addr()).c_str());
    _envvar[i++] = strdup(std::string("REMOTE_PORT=" + _req->get_client_port()).c_str());
    _envvar[i++] = strdup(std::string("REQUEST_METHOD=" + request_method).c_str());
    _envvar[i++] = strdup(std::string("REQUEST_URI=" + request_uri).c_str());
    _envvar[i++] = strdup(std::string("SCRIPT_NAME=" + _req->get_path()).c_str());
    _envvar[i++] = strdup(std::string("SCRIPT_FILENAME=" + _resource_path).c_str());
    _envvar[i++] = strdup("SERVER_ADDR=172.0.0.1");
    _envvar[i++] = strdup(std::string("SERVER_NAME=" + _host).c_str());
    _envvar[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    _envvar[i++] = strdup("SERVER_SOFTWARE=webserv");   
    _envvar[i++] = strdup(std::string(
			      "SERVER_PORT=" +
			      n_to_str(_req->get_server_port())
			      ).c_str()
	);
    _envvar[i] = NULL;
}

void CGI::set_args(void) {
    std::string uri = Response::get_filename_from_uri(_resource_path);
    
    _args[0] = (_cgi_path.empty()) ? strdup(uri.c_str()) : strdup(_cgi_path.c_str());
    _args[1] = (_cgi_path.empty()) ? NULL : strdup(uri.c_str());
    _args[2] = NULL;
}

std::string CGI::set_path_info(void) {
    size_t path_separator = _resource_path.rfind('/');

    return (path_separator == std::string::npos) ?
	"/" :
	_resource_path.substr(0, path_separator);
}

/* CGIs imprimen como fin de linea tanto LF como CRLF: preparamos la respuesta para normalizarla a un único estándar (LF) */
void CGI::parse_normalize(void) {
    size_t i = 0;

    while ((i = _raw_response.find("\r\n", i)) != std::string::npos) {
	_raw_response.erase(i, 1);
    }
}

/*  Da formato a la respuesta recibida por el CGI y comprueba posibles errores sintácticos:
 * Una respuesta proveniente de un CGI ha de tener al menos un CGI-Header; si
 no está presente el servidoro se repiten lanza respuesta 500 (internal server error)
 * CGI-Header: Content-Type / Location / Status
 * Content-Type DEBE estar presente si existe un cuerpo
 * La presencia de un header Location genera una respuesta 302 (Found)
 * Location puede ser una redirección por parte del servidor o del cliente
 * La presencia de un header Status sobreescribe el estado anterior de la respuesta
 * El código de estado por defecto es 200 OK
 */

void CGI::parse_response_headers(const std::string& headers) {
    static const std::string cgi_header[3] = { "Content-Type", "Location", "Status"};

    std::stringstream headers_ss(headers);
    std::string       header_line;

    while (std::getline(headers_ss, header_line)) {
        size_t separator = header_line.find(':');

        if (separator == std::string::npos) {
            continue ;
        }
        std::string header_field(header_line.substr(0, separator++));
        std::string header_value(header_line.substr(header_line.find_first_not_of(' ', separator)));

        if (header_value.empty()) {
            continue ;
        }
	if (!header_field.compare("Content-type")) {
	    header_field = "Content-Type";
        }
        for (int i = 0; i < 3; i++) {
            if (!header_field.compare(cgi_header[i])) {
                if (_header_map.find(header_field) != _header_map.end()) {
                    throw (StatusLine(500, REASON_500, "CGI: parse(), duplicate CGI-header in CGI response"));
                }
                break ;
            }
        }
        _header_map.insert(std::pair<std::string, std::string>(header_field, header_value));
    }
    if (_header_map.find(cgi_header[0]) == _header_map.end() &&
	_header_map.find(cgi_header[1]) == _header_map.end() &&
	_header_map.find(cgi_header[2]) == _header_map.end()) {
        throw (StatusLine(500, REASON_500, "CGI: parse(), missing necessary CGI-Header in response"));
    }
}

void CGI::parse_response_body(const std::string& body) {
    if (!body.empty() && _header_map.find("Content-Type") == _header_map.end()) {
        throw (StatusLine(500, REASON_500, "CGI: parse(), missing body in response with Content-Type header defined"));
    }
    std::map<std::string, std::string>::const_iterator cl = _header_map.find("Content-Length");
    if (cl != _header_map.end()) {
        char *ptr;

        long length = strtol(cl->second.c_str(), &ptr, 0);
        if (length < 0 || *ptr || length != static_cast<long>(body.size())) {
            throw (StatusLine(500, REASON_500, "CGI: parse(), bad content-length definition in CGI response"));
        }
    }
    _body_string = body;
}

/* Status = "Status:" status-code SP reason-phrase NL */

void CGI::parse_status_line(void) {
    std::map<std::string, std::string>::const_iterator lc(_header_map.find("Location"));
    std::map<std::string, std::string>::iterator st(_header_map.find("Status"));

    if (st != _header_map.end()) {
        std::stringstream status_ss(st->second);
        std::string code, reason;
        char *ptr;

        status_ss >> code;
	std::getline(status_ss, reason);
        long status_code = strtol(code.c_str(), &ptr, 0);

        if (!(status_code >= 100 && status_code < 600) || *ptr) {
            throw StatusLine(500, REASON_500, "CGI: parse(), malformed Status-Header");
        }
        _status_line = StatusLine(static_cast<int>(status_code), reason.c_str(), "CGI-defined Status line");
        _header_map.erase(st);
    }
    if (lc != _header_map.end()) {
        _status_line = StatusLine(302, REASON_302, "CGI: redirection");
        return ;
    }
}

void CGI::parse_response(void) {
    parse_normalize();
    size_t separator = _raw_response.find("\n\n");

    if (separator == std::string::npos) {
        throw (StatusLine(500, REASON_500, "CGI: parse(), bad syntax in CGI repsonse"));
    }
    std::string headers = _raw_response.substr(0, separator);
    std::string body = _raw_response.substr(separator + 2);

    parse_response_headers(headers);
    parse_response_body(body);
    parse_status_line();
}

CGI::CGI(Request *req, const std::string& uri, const cgi_pair& cgi_info) :
    _req(req), _status_line(200, REASON_200, "CGI-generated response") {
    /* inicializa atributos de CGI (variables de entorno, argumentos) */
    for (int i = 0; i < 2; i++) {
        _fdIN[i] = -1;
        _fdOut[i] = -1;
    }

    // GET : QUERY_STRING + PATH_INFO 
    // POST : PATH_INFO + CONTENT_length 
    if ((_envvar = new char*[N_ENV_VAR]) == NULL) {
        throw std::runtime_error("Error on a cgi malloc\n");
    }
    // ** Set args for execve **
    if ((_args = new char*[3]) == NULL) {
        throw std::runtime_error("CGI: " + std::string(strerror(errno)) + "\n");
    }

    /* llamada a getcwd: cambio de malloc a reservar memoria en stack */
    /* por problemas de fugas de memoria                              */
    char cwd[S_BUFFR_PWD];
    if (getcwd(cwd, S_BUFFR_PWD) == NULL) {
        throw std::runtime_error("CGI: getcwd() call failure");
    }
    _host = set_host();
    _document_root = set_document_root(_req->get_location(), cwd);
    _resource_path = set_resource_path(uri, cwd);
    _cgi_path = set_cgi_path(cgi_info.second);
    _path_info = set_path_info();

    set_args();
    set_env_variables();
}

CGI::~CGI()
{
    int i = -1;
    while (_envvar[++i]){
        free(_envvar[i]); _envvar[i] = NULL;}
    delete[] _envvar;

    i = -1;
    while (_args[++i]){
        free(_args[i]); _args[i] = NULL;}
    delete[] _args;

    close_fdIN();
    close_fdOut();
}

void CGI::executeCGI_read(void) {
    char buf[S_BUFFR_CGI_PIPE + 1] = {0};
    int rd_out;
   
    while ((rd_out = read(_fdOut[0], buf, S_BUFFR_CGI_PIPE)) > 0)
    {
        _raw_response.append(buf, rd_out);
        memset(buf, 0, S_BUFFR_CGI_PIPE + 1);
    }
    if (rd_out == -1) {
        throw StatusLine(500, REASON_500, std::string("CGI: read() - ") + strerror(errno));
    }
}

void CGI::executeCGI()
{
    if (pipe(_fdOut) < 0 || pipe(_fdIN) < 0) {
        throw StatusLine(500, REASON_500, "pipe failed in executeCGI method");
    }

    pid_t pid = fork();
    if (pid == -1) {
        throw StatusLine(500, REASON_500, "CGI: fork() call error");
    }
    if (!pid){
        if (dup2(_fdOut[1], STDOUT_FILENO) == -1 ||
	    dup2(_fdIN[0], STDIN_FILENO) == -1) {
	    std::cerr << std::string("[ cgi error ] ") + strerror(errno) << "\n";
	    throw std::exception();
	};
        close_fdOut();
        close_fdIN();
        if (chdir(_path_info.c_str()) == -1 ||
	    execve(_args[0], _args, _envvar) == -1) {
	    std::cerr << std::string("[ cgi error ] ") + strerror(errno) << "\n";
	    throw std::exception();
        }
    }
    close(_fdOut[1]);
    _fdOut[1] = -1;

    if (_req->get_method() == POST) {
	if (ft_write(_fdIN[1], _req->get_body_string(), _req->get_body_size()) == -1) {
	    throw StatusLine(500,
			     REASON_500,
			     std::string("CGI: write() - ") + strerror(errno)
		);
	}
    }
    close_fdIN();
    executeCGI_read();
    close_fdOut();
    // Checking if execve correctly worked
    int status = 0;
    
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == EXECVE_FAIL)
    {
        throw StatusLine(500, REASON_500, "execve failed in executeCGI method");
    }
}

std::string CGI::getHeaders(void) const {
    std::string header_string;

    for (std::map<std::string, std::string>::const_iterator h = _header_map.begin(); h != _header_map.end(); h++) {
        header_string.append(h->first + ": " + h->second + "\n");
    }
    /* si escribimos el doble salto de línea aquí, llamamos a esta función después de escribir los
     * headers de la respuesta en execCGI */
    header_string.append("\n");
    return header_string;
}

std::string CGI::getBody(void) const {
    return _body_string;
}

const StatusLine& CGI::getStatusLine(void) const {
    return _status_line;
}

bool CGI::isHeaderDefined(const std::string& header_field) const {
    return (_header_map.find(header_field) != _header_map.end());
}

void CGI::mySwap(CGI &a, CGI &b)
{
    std::swap(a._envvar, b._envvar);
    std::swap(a._req, b._req);
    std::swap(a._path_info, b._path_info);
    std::swap(a._raw_response, b._raw_response);
    std::swap(a._status_line, b._status_line);
    std::swap(a._header_map, b._header_map);
    std::swap(a._body_string, b._body_string);
}

CGI::CGI(void) :  _status_line(200, REASON_200, "CGI-generated response") {
    for (int i = 0; i < 2; i++) {
        _fdIN[i] = -1;
        _fdOut[i] = -1;
    }
}

CGI::CGI(const CGI& other) : 
    _req(other._req), _path_info(other._path_info), _raw_response(other._raw_response),
    _status_line(other._status_line), _header_map(other._header_map), _body_string(other._body_string) {
    _envvar = new char*[7];
    _args = new char*[3];
    if (_args == NULL || _envvar == NULL) {
        throw std::runtime_error("CGI: " + std::string(strerror(errno)) + "\n");
    }
    int i = 0;

    for (; other._envvar[i] != NULL; i++) {
        _envvar[i] = strdup(other._envvar[i]);
    }
    _envvar[i] = NULL;
    for (i = 0; other._args[i] != NULL; i++ ) {
        _args[i] = strdup(other._args[i]);
    }
    if (i == 1) _args[i] = NULL;
    _args[2] = NULL;
}
