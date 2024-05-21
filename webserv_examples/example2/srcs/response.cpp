#include "response.hpp"

FileParser::FileParser(void) : rawFile(), status(false) { }

FileParser::FileParser(const FileParser& other) : rawFile(other.rawFile), status(other.status) { }

FileParser::FileParser(const std::string& filePath, bool _status) { 
    status = _status;

    std::ifstream file(filePath.c_str());
    if (!file.is_open()) {
	throw StatusLine(500, REASON_500, "FileParser: error trying to get body file");
    }
    rawFile = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
}

FileParser::~FileParser() { }

std::string FileParser::getRequestFile(void) const {
    return rawFile;
}

int FileParser::getRequestFileSize(void) const {
    return rawFile.size();
}

/* ------------------------ COPLIEN FORM ----------------------- */

Response::Response() : _req(NULL), _autoIndex(false), _endConn(false) {}

Response::Response(const Response& c) : 
    _req(c._req), _staLine(c._staLine), _buffer(c._buffer),
    _autoIndex(c._autoIndex), _endConn(c._endConn) {}

Response::~Response() { }

Response& Response::operator=(Response a)
{
    swap(a, *this);
    return *this;
}

/* --------------------------- SETTERS ------------------------- */

void Response::setRequest(Request* req)
{
    _req = req;
}

void Response::setStatusLine(const StatusLine& staLine)
{
    _staLine = staLine;
}

void Response::setLocation(const Location& loc)
{
    _loc = loc;
}

/* --------------------------- GETTERS ------------------------- */

const StatusLine& Response::getStatusLine() const
{
    return _staLine;
}

int Response::getCode() const
{
    return _staLine.getCode();
}

const std::string& Response::getBuffer() const
{
    return _buffer;
}

bool Response::getEndConn() const 
{
    return _endConn;
}

/* --------------------------- METHODS ------------------------- */

void Response::clear()
{
    _req = NULL;
    _staLine.clear();
    _buffer.clear();
    _autoIndex = false;
}

void Response::fillBuffer(Request* req, const Location& loc, const StatusLine& sl)
{
    setRequest(req);
    setLocation(loc);
    setStatusLine(sl);

    if (_staLine.getCode() == 100)
    {
	return setUp100Continue();
    }
    if (_staLine.getCode() >= 400)
    {
	return fillError(_staLine);
    }
    try
    {
	std::string hostName(_req->get_headers().find("Host")->second); 		// Keeping only host name and removing port
	hostName = hostName.substr(0, hostName.find(':'));

	if (!_loc.get_return_uri().empty())        // Doing an HTTP redirection (301) if redirect field filled in matched location block
        {
	    std::string redirectedUri = _req->get_path();            // Replacing location name in the URI with the redirect string set in config file
	    replaceLocInUri(redirectedUri, _loc.get_return_uri());

	    // Replacing previous requested URI with redirected URI for next client request
	    // (Location header in 301 response will be set with this URI)
	    _req->set_path(redirectedUri);

	    throw StatusLine(301, REASON_301, "http redirection");
        }
	std::string realUri = reconstructFullURI(_req->get_method(), _req->get_path());    // Modifying URI with root and index directive if any, checking for the allowed methods
	//cgi_pair cgiConfig = getCgiExecutableName(realUri);     					    // Checking if the targeted file is a CGI based on his extension

	_endConn = isMarkedForClosing();
	// Execute the appropriate method and fills the response buffer with status line + 
	// headers + body (if any). If an error occurs during this process, it will throw 
	// a StatusLine object with the appropriate error code.
	if (_req->get_method() == GET || _req->get_method() == HEAD)
	    execGet(realUri);
	else if (_req->get_method() == POST)
	    execPost(realUri);
	else if (_req->get_method() == PUT)
	    execPut(realUri);
	else if (_req->get_method() == DELETE)
	    execDelete(realUri);
	else
	    throw (StatusLine(400, REASON_400, "request method do not exist")); /* doble check de method */
    }
    catch (const StatusLine& errorStaLine)	// If an error occured during the reponse creation
    {
	fillError(errorStaLine);
    }
}


/* ----------------------- PRIVATE METHODS --------------------- */

void Response::execCgi(const std::string& realUri, const cgi_pair& cgiConfig)
{
    struct stat st;
    if (stat(realUri.c_str(), &st) == -1)
    {
	throw StatusLine(404, REASON_404, "fillCgi method: " + realUri);
    }

    CGI cgi(_req, realUri, cgiConfig);
    cgi.executeCGI();
    cgi.parse_response();

    if (cgi.getStatusLine().getCode() == 400) {
	_staLine = StatusLine(500, REASON_500, "CGI: received bad request");
    } else {
	_staLine = cgi.getStatusLine();
    }
    fillStatusLine(_staLine);
    if (cgi.isHeaderDefined("Server") == false) {
	fillServerHeader();
    }
    if (cgi.isHeaderDefined("Date") == false) {
	fillDateHeader();
    }
    if (!cgi.isHeaderDefined("Content-Length") &&
	client_expects_body_in_response()) {
	fillContentlengthHeader(n_to_str(cgi.getBody().size()));
    }
    _buffer += cgi.getHeaders();
    if (_req->get_method() != HEAD) {
	_buffer += cgi.getBody();
    }
}

/* POST sólo trabaja a través de CGIs */
void Response::execPost(const std::string& realUri)
{
    cgi_pair cgi_info = getCgiExecutableName(realUri);

    if (!cgi_info.first.empty())
    {
	execCgi(realUri, cgi_info);
    }
    else 
    {
	throw (StatusLine(403, REASON_403, "POST: posting to a static file (or script not allowed by cgi_pass"));
    }
}

void Response::fillContentlengthHeader(const std::string& size) 
{
    _buffer += "Content-Length: " + size + CRLF;
}

void Response::fillServerHeader() 
{
    _buffer += "Server: webserv\r\n";
}

void Response::fillDateHeader() 
{
    time_t now = time(0);			 // current date and time on the current system

    char* date_time = ctime(&now); 	// convert now to string form

    std::vector<std::string> date = splitWithSep(date_time, ' ');	// Splitting date line and removing '\n'

    date.back().resize(4);

    // Formating header date.
    // ctime format = Thu May 20 14:33:40 2021 >> to header date format : Thu, 20 May 2021 12:16:42 GMT
    _buffer += "Date: " + date[0] + ", " + date[2] + " " + date[1] + " " + date[4] + " " + date[3] + " GMT" + CRLF;
}

void Response::fillLastModifiedHeader(const char* uri)
{
    struct stat infFile;

    if (stat(uri, &infFile) == -1)
	throw StatusLine(404, REASON_404, "fillLastModifiedHeader method");

    struct tm* lm = localtime(&infFile.st_mtime);

    const std::string day[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const std::string mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    _buffer += "Last-Modified: " + day[lm->tm_wday] + ", " + n_to_str(lm->tm_mday) + " " + mon[lm->tm_mon] + " " 
	+ n_to_str(lm->tm_year + 1900) + " " + n_to_str(lm->tm_hour) + ":" + 
	n_to_str(lm->tm_min) + ":" + n_to_str(lm->tm_sec) + " GMT" + CRLF;
}

void Response::fillLocationHeader(const std::string& redirectedUri)
{
    _buffer += "Location:" + redirectedUri + CRLF;
}

void Response::fillStatusLine(const StatusLine& staLine)
{
    _buffer = "HTTP/1.1 " + n_to_str(staLine.getCode()) + " " + staLine.getReason();
    if (!staLine.getAdditionalInfo().empty())
	_buffer += " (" + staLine.getAdditionalInfo() + ")";
    _buffer += CRLF;
}

void print_Loc(const Location& _loc)
{
    std::cout << "root: " << _loc.get_root() << "\n";
    for (std::vector<std::string>::const_iterator it = _loc.get_methods().begin(); it != _loc.get_methods().end(); it++)
	std::cout << "methods: " << *it << "\n";
		
    for (std::vector<std::string>::const_iterator it = _loc.get_index().begin(); it != _loc.get_index().end(); it++)
	std::cout << "index: " << *it << "\n";
}

void Response::replaceLocInUri(std::string& uri, const std::string& root)
{
    if (!root.empty() && root.at(root.size() - 1) != '/' \
        && !uri.empty() && uri.at(0) != '/') {
	uri.insert(0, 1, '/');
    }
    if (!root.empty() && root[root.size() - 1] == '/' \
	&& !uri.empty() && uri.at(0) == '/') {
	uri.erase(0, 1);
    }	
    uri.insert(0, root);
}

std::string Response::addIndex(const std::string& uri, const std::vector<std::string>& indexs)
{
    struct stat infFile;
    for (std::vector<std::string>::const_iterator it = indexs.begin();
	 it != indexs.end();
	 ++it
	)
    {
	std::string uriWithIndex(uri + *it);

	if (!stat(uriWithIndex.c_str(), &infFile)) {
	    return uriWithIndex;
	}
    }
    if (_loc.get_autoindex()) {
	_autoIndex = true;
	return uri;
    }
    throw StatusLine(403, REASON_403, "addIndex: trying to access a directory " + uri);
}

void Response::checkMethods(int method, const std::vector<std::string>& methodsAllowed) const
{
    std::string tab[NB_METHODS] = { "GET", "HEAD", "POST", "DELETE", "PUT" };

    for (std::vector<std::string>::const_iterator it = methodsAllowed.begin();
	 it != methodsAllowed.end(); ++it)
	if (!it->compare(tab[method]))
	    return ;

    throw StatusLine(405, REASON_405, "checkMethods method");
}

std::string Response::reconstructFullURI(int method, std::string uri)
{
    struct stat infFile;

    checkMethods(method, _loc.get_methods());
    if (_loc.get_alias() == true && _loc.uri.size() > 1) {
	uri.erase(0, _loc.uri.size());
    }
    replaceLocInUri(uri, _loc.get_root());
    if (stat(uri.c_str(), &infFile) == -1 && method != PUT) {
	throw (StatusLine(404, REASON_404, "reconstructFuLLURI: file " + uri + " does not exist"));
    }
	
    if (method == PUT) {
	std::string upload_uri(get_filename_from_uri(uri));

	replaceLocInUri(upload_uri, _loc.get_upload_path());
	replaceLocInUri(upload_uri, _loc.get_root());
	if (_loc.get_upload_path().empty()) {
	    throw (StatusLine(403, REASON_403, "PUT: method allowed, but no upload_path was setted server configuration"));
	}
	if (S_ISDIR(infFile.st_mode)) {
	    throw (StatusLine(403, REASON_403, "PUT: tried to upload a directory"));
	}
	return upload_uri;
    }
    // works addIndex throw a 403 error StatusLine object
    if (S_ISDIR(infFile.st_mode))
    {
	if (!isResourceAFile(uri)) // redirect directories not ended in '/'
	{
	    _req->set_path(_req->get_path() + "/");
	    throw StatusLine(301, REASON_301, "redirect " + uri + " to complete directory uri");
	}
	if (!(method == DELETE))
	{
	    uri = addIndex(uri, _loc.get_index());
	}
    }
    return uri;
}

void Response::fillError(const StatusLine& sta)
{
    if (sta.getCode() != _staLine.getCode()) {
	_staLine = sta;
    }
    _buffer.clear();
    if (_staLine.getCode() == 404 && !_loc.get_error_page().empty()) {
	std::string error_uri = _loc.get_error_page(), error_abs_path;
	struct stat file_inf;
	  
	if (error_uri.at(0) != '/') {
	    std::string error_path = _req->get_path().substr(0, _req->get_path().rfind('/'));

	    if (error_path.empty() || error_path.at(error_path.size() - 1) != '/') {
		error_path.push_back('/');
	    }
	    error_uri.insert(0, error_path);
	}
	error_abs_path = error_uri;
	replaceLocInUri(error_abs_path, _loc.get_root());
	if (stat(error_abs_path.c_str(), &file_inf) != -1) {
	    _staLine = StatusLine(302, REASON_302, "fillError(): redirecting to default error page");
	    _req->set_path(error_uri);
	}
    }
    std::string error_file = getErrorPage(_staLine);

    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();

    if (_staLine.getCode() == 301 || _staLine.getCode() == 302) {
	fillLocationHeader(_req->get_path());
    }
    fillContentTypeHeader();

    if (_req->get_method() != HEAD) {
	fillContentlengthHeader(n_to_str(error_file.size()));
	_buffer += CRLF + error_file;
    } else {
	_buffer += CRLF;
    }
}

bool Response::isResourceAFile(const std::string& uri) const {
    return (!uri.empty() && uri.at(uri.size() - 1) == '/');
}

void Response::execGet(const std::string& realUri)
{
    cgi_pair cgi_info = getCgiExecutableName(realUri);

    if (!cgi_info.first.empty()) {
	execCgi(realUri, cgi_info);
	return ;
    }
    // Storing status line and some headers in buffer
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();
    
    if (!isResourceAFile(realUri)) {
	FileParser body(realUri.c_str(), true); // CAHNGER

	fillContentlengthHeader(n_to_str(body.getRequestFileSize()));
	fillLastModifiedHeader(realUri.c_str());

	// For GET, writing the body previously stored to the buffer
	if (_req->get_method() == GET) {
	    fillContentTypeHeader(getResourceExtension(realUri));
	    _buffer += CRLF + body.getRequestFile();
	}
	return ;
    }
    if (_autoIndex == true)  {
	std::string autoIndexPage;
	autoIndexDisplayer(realUri, autoIndexPage);
	fillContentTypeHeader();
	fillContentlengthHeader(n_to_str(autoIndexPage.size()));
	_buffer += CRLF + autoIndexPage;
	return ;
    }
    throw StatusLine(403, REASON_403, "GET: requested directory with autoindex set off");
}

void Response::execPut(const std::string& realUri)
{
    if (_loc.get_upload_path().empty()) {
	throw StatusLine(403, REASON_403, "PUT: method is accepted but upload_path is not set in configuration file");
    }
    struct stat infoFile;
    int fileExists = stat(realUri.c_str(), &infoFile);
	
    if (!fileExists && (S_ISDIR(infoFile.st_mode))) {
	throw StatusLine(403, REASON_403, "PUT: trying to upload a file when not allowed");
    }
    _staLine = (!fileExists) ?
	StatusLine(204, REASON_204, "PUT: update existing resource") :
	StatusLine(201, REASON_201, "PUT: create new resource");

    std::fstream postFile(realUri.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (!postFile.is_open()) {
	throw StatusLine(500, REASON_500, "PUT: failed to open/create new resource");
    }
    postFile << _req->get_request_body().get_body();
    postFile.close();

    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();
    if (_staLine.getCode() == 201) 
    {
	fillLocationHeader(realUri);
	fillContentlengthHeader(n_to_str(_req->get_request_body().get_body().size()));
	_buffer += CRLF + _req->get_request_body().get_body();
    } 
    else
    {
	_buffer += CRLF;
    }
}

void Response::execDelete(const std::string& realUri)
{
    if (remove(realUri.c_str()))
    {
	throw (StatusLine(500, REASON_500, "remove function failed in DELETE method"));
    }
    _staLine = StatusLine(204, REASON_204, "DELETE: resource removed successfully");
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();
    _buffer += CRLF;
}

void Response::autoIndexDisplayer(const std::string& realUri, std::string& autoIndexPage) {
    autoIndexPage.append("<html>\n<head><title>Index of " + realUri + "</title></head>\n");
    autoIndexPage.append("<body><h1>Index of " + realUri + "</h1><hr><br><pre><ul>");

    DIR* 			dir_ptr;
    struct dirent*	dir_s;

    std::vector<std::string> file_list;
    dir_ptr = opendir(realUri.c_str());
    if (!dir_ptr) {
	throw StatusLine(500, REASON_500, "autoindex: could not open directory");
    }
    while ((dir_s = readdir(dir_ptr)) != NULL) {
	file_list.push_back(dir_s->d_name);
    }
    closedir(dir_ptr);

    for (std::vector<std::string>::iterator it = file_list.begin(); it != file_list.end(); it++) {
	struct stat info;
	std::string filePath = realUri + *it;

	if (stat(filePath.c_str(), &info) == -1) {
	    StatusLine(500, REASON_500, "autoindex: could not open file");
	}
	std::string timeStamp = asctime(localtime(&info.st_mtime));

	timeStamp.erase(--timeStamp.end());
	std::string fileName = (it->size() > 10) ? it->substr(0, 15) + "." : *it;
	std::string spaceFirst = std::string("                    ").substr(fileName.size());
	std::string fileSize = n_to_str(info.st_size);
	std::string spaceSecond = std::string("          ").substr(fileSize.size());

	autoIndexPage.append("<li><a href=\"" + *it + "\">" + fileName + "</a>" 
			     + spaceFirst + fileSize + spaceSecond + timeStamp + "</li>\n");
    }
    autoIndexPage.append("</ul></pre><br></body></html>");
}

std::vector<std::string> Response::splitWithSep(const std::string& str, char dlm) {
    std::vector<std::string> tab;
    std::stringstream strStream(str.c_str());
    std::string token;

    while (std::getline(strStream, token, dlm)) {
	tab.push_back(token);
    }
    return tab;
}

cgi_pair Response::getCgiExecutableName(const std::string& uri) const {
    std::string fileExt(getResourceExtension(uri));
    if (!fileExt.compare("cgi")) {
	return cgi_pair(".cgi", "");
    }
    for (cgi_iterator it = _loc.get_cgi_pass().begin(); it != _loc.get_cgi_pass().end(); it++) {
	if (!fileExt.compare(it->first)) {
	    return *it;
	}
    }
    return cgi_pair("","");
}

std::string Response::getErrorPage(const StatusLine& sl) {
    std::string errorPage;

    errorPage.append("<!DOCTYPE><html><head><title>Webserv</title></head><body><h1>Webserv</h1><hr><p>");
    errorPage.append(sl.getReason());
    errorPage.append("</p></body></html>");
    return errorPage;
}

/* overload for webserver-generated pages */
void Response::fillContentTypeHeader(void) {
    _buffer.append("Content-Type: text/html");
    _buffer.append(CRLF);
}

void Response::fillContentTypeHeader(const std::string& fileExt) {
    std::string contentTypeValue = "application/octet-stream"; // default value for unrecognized file extesions
    
    _buffer.append("Content-Type: ");
    if (fileExt.empty()) {
	_buffer.append(contentTypeValue + CRLF);
	return ;		
    }

    /* content-type should be set according to accept header values; if
       accept header is present in request and does not allow response uri file format,
       server should send 406 code error, but this behaviour is optional */
    const std::string type[5] = {"application", "audio", "image", "text", "video"};

    const std::string appl_subtype[1] = {"javascript"};
    const std::string appl_fileext[1] = {"js"};
    
    const std::string audio_subtype[1] = {"mpeg"};
    const std::string audio_fileext[1] = {"mp3"};

    const std::string image_subtype[6] = {"gif", "jpeg", "jpeg", "png", "tiff", "svg+xml"};
    const std::string image_fileext[6] = {"gif", "jpg",  "jpeg", "png", "tiff", "svg"};

    const std::string text_subtype[5] = {"css", "csv", "html", "plain", "xml"};
    const std::string text_fileext[5] = {"css", "csv", "html", "txt",   "xml"};

    const std::string video_subtype[5] = {"mpeg", "mp4", "quicktime", "x-flv", "webm"};
    const std::string video_fileext[5] = {"mpeg", "mp4", "mov",       "flv",   "webm"};

    const std::string* subtype_arr[5] = {appl_subtype, audio_subtype, image_subtype, text_subtype, video_subtype};
    const std::string* fileext_arr[5] = {appl_fileext, audio_fileext, image_fileext, text_fileext, video_fileext};

    int subtype_id[5] = {1, 1, 6, 5, 5};

    for (int i = 0; i < 5; i++) {
	for (int j = 0; j < subtype_id[i]; j++) {
	    if (!fileext_arr[i][j].compare(fileExt)) {
		contentTypeValue = type[i] + "/" + subtype_arr[i][j];
		break ;
	    }
	}
    }
    _buffer.append(contentTypeValue + CRLF);
}

std::string Response::getResourceExtension(const std::string& uri) const {
    std::string ext("");
    size_t	slash_pos = uri.rfind('/'), dot_pos;

    if (slash_pos == std::string::npos) {
	return ext;
    }
    dot_pos = uri.rfind('.');
    if (dot_pos == std::string::npos) {
	return ext;
    }
    ext = uri.substr(dot_pos + 1);
    return ext;
}

std::string Response::get_filename_from_uri(const std::string& uri) {
    size_t slash_pos = uri.rfind('/');

    if (slash_pos == std::string::npos) {
	return ""; // bad format
    }
    std::string resource_name = uri.substr(++slash_pos);
    return resource_name;
}

bool Response::isMarkedForClosing(void) const
{
    std::map<std::string, std::string>::const_iterator it = _req->get_headers().find("Connection");
    if (it != _req->get_headers().end() && !it->second.compare("close")) {
	return true;
    }
    return false;
}

void Response::setUp100Continue(void)
{
    fillStatusLine(_staLine);
    fillServerHeader();
    fillDateHeader();

    _buffer += CRLF;
}

bool Response::client_expects_body_in_response(void) const {
    return (_staLine.getCode() >= 200) &&
	(_staLine.getCode() != 204)    &&
	(_staLine.getCode() != 304)    &&
	(_req->get_method() != HEAD);
}

/* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

void swap(Response& a, Response& b)
{
    std::swap(a._req, b._req);
    swap(a._staLine, b._staLine);
    std::swap(a._buffer, b._buffer);
    std::swap(a._autoIndex, b._autoIndex);
    std::swap(a._endConn, b._endConn);
}
