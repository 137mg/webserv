#include "location.hpp"

Location::Location(void) : _alias(false) { }

Location::Location(const Location& other) {
  *this = other;
}

Location::Location(const location_block_t& loc_b) : _alias(false)  {
  typedef std::vector<std::string> string_vector;

  uri = loc_b.uri;
  if (!loc_b.dir[D_ERROR_PAGE].empty()) {
    _error_page = loc_b.dir[D_ERROR_PAGE].front();
  }
  if (!loc_b.dir[D_ROOT].empty()) {
    _root = loc_b.dir[D_ROOT].front();
  }
  if (!loc_b.dir[D_ALIAS].empty()) {
    _root = loc_b.dir[D_ALIAS].front();
    _alias = true;
  }
  if (!loc_b.dir[D_RETURN].empty()) {
    _return_uri = loc_b.dir[D_RETURN].front();
  }
  if (!loc_b.dir[D_UPLOAD].empty()) {
    _upload_path = loc_b.dir[D_UPLOAD].front();
  }
  for (string_vector::const_iterator it = loc_b.dir[D_INDEX].begin(); it != loc_b.dir[D_INDEX].end(); it++) {
    if (std::find(_index.begin(), _index.end(), *it) == _index.end()) {
      _index.push_back(*it);
    }
  }
  for (string_vector::const_iterator it = loc_b.dir[D_METHOD].begin(); it != loc_b.dir[D_METHOD].end(); it++) {
    if (std::find(_accept_method.begin(), _accept_method.end(), *it) == _accept_method.end()) {
      _accept_method.push_back(*it);
    }
  }
  for (string_vector::const_iterator it = loc_b.dir[D_CGI_PASS].begin(); it != loc_b.dir[D_CGI_PASS].end(); it++) {
    if ((it + 1) == loc_b.dir[D_CGI_PASS].end()) throw std::runtime_error("si ves esto el parser está roto\n");
    _cgi_pass.push_back(std::pair<std::string, std::string>(*it, *++it));
  }
  _body_size = std::atoi(loc_b.dir[D_BODY_SIZE].front().c_str());
  _autoindex = !loc_b.dir[D_AUTOINDEX].front().compare("on") ? true : false;
}

Location::~Location() { }

Location& Location::operator=(const Location& other) {
  if (this == &other) {
    return *this;
  }
  uri = other.uri;
  _error_page = other._error_page;
  _root = other._root;
  _return_uri = other._return_uri;
  _alias = other._alias;
  _upload_path = other._upload_path;
  _index = other._index;
  _accept_method = other._accept_method;
  _cgi_pass = other._cgi_pass;
  _body_size = other._body_size;
  _autoindex = other._autoindex;
  return *this;
}

const std::string& Location::get_error_page(void) const {
  return _error_page;
}

const std::string& Location::get_root(void) const {
  return _root;
}

const std::string& Location::get_return_uri(void) const {
  return _return_uri;
}

const std::string& Location::get_upload_path(void) const {
  return _upload_path;
}

const std::vector<std::string>& Location::get_index(void) const {
  return _index;
}

const std::vector<std::string>& Location::get_methods(void) const {
  return _accept_method;
}

const std::vector<std::pair<std::string, std::string> >& Location::get_cgi_pass(void) const {
  return _cgi_pass;
}

unsigned int Location::get_body_size(void) const {
  return _body_size;
}

bool Location::get_autoindex(void) const {
  return _autoindex;
}

bool Location::get_alias(void) const {
  return _alias;
}
