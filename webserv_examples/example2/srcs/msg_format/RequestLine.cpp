#include "msg_format/RequestLine.hpp"

RequestLine::RequestLine() : _method(-1) { }

RequestLine::RequestLine(const RequestLine& c) :
    _method(c._method),
    _path(c._path),
    _virtual_path(c._virtual_path),
    _query(c._query) { }

RequestLine::~RequestLine() { }

RequestLine& RequestLine::operator=(RequestLine a)
{
  swap(*this, a);
  return *this;
}

int RequestLine::get_method() const 
{ 
  return _method; 
}

const std::string& RequestLine::get_path() const 
{ 
  return _path; 
}

const std::string& RequestLine::get_virtual_path(void) const
{
    return _virtual_path;
}

const std::string& RequestLine::get_query() const 
{ 
  return _query; 
}
		
void RequestLine::set_method(int method) 
{ 
  _method = method;
}

void RequestLine::set_path(const std::string& path) 
{ 
  _path = path;
}

void RequestLine::set_query(const std::string& query) 
{ 
  _query = query;
}

void RequestLine::clear()
{
  _method = -1;
  _path.clear();
  _virtual_path.clear();
  _query.clear();
}

bool RequestLine::empty() const 
{
  return _path.empty();
}

/* set_path wrapper para tratar llamadas a scripts */
void RequestLine::parse_path(
    const std::string& req_path,
    const std::vector<cgi_pair>& cgi_pass_v
    ) {
    if (!cgi_pass_v.empty()) {
	for (std::vector<cgi_pair>::const_iterator it = cgi_pass_v.begin();
	     it != cgi_pass_v.end();
	     it++
	    ) {
	    /* Identifica llamada a script en el path     */
	    /* '.cgi_ext/' marca el inicio de path virtual*/
	    
	    std::string cgi_id("." + it->first + "/");
	    size_t      cgi_pos(req_path.find(cgi_id));

	    if (cgi_pos != std::string::npos) {
		size_t v_path_pos = cgi_pos + (cgi_id.size() - 1);
		
		_virtual_path = req_path.substr(v_path_pos);
		set_path(req_path.substr(0, v_path_pos));
		return ;
	    }
	}
    }
    set_path(req_path);
}

void RequestLine::print() const
{
  std::cout << "------ REQUEST LINE ------\nmet: " << _method << ", path: |" << _path
	    << "|, query: |" << _query << "|\n";
}

void swap(RequestLine& a, RequestLine& b)
{
  std::swap(a._method, b._method);
  std::swap(a._path, b._path);
  std::swap(a._virtual_path, b._virtual_path);
  std::swap(a._query, b._query);
}

inline std::ostream& operator<<(std::ostream& stream, const RequestLine& reqLine)
{
  stream << "Method = " << reqLine.get_method() << ", path = |" << reqLine.get_path() << "|";
	
  if (!reqLine.get_query().empty())
    stream << ", query = |" << reqLine.get_query() << "|";
		
  return stream;
}
