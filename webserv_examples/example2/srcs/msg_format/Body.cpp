#include "msg_format/Body.hpp"

Body::Body() : _size(0), _maxSize(0), stage(SIZE) {}

Body::Body(const Body& c) :
    _size(c._size), _maxSize(c._maxSize), _buffer(c._buffer), stage(SIZE) {}

Body::~Body() {}

Body& Body::operator=(Body a)
{
    swap(*this, a);
    return *this;
}

const std::string& Body::get_body() const 
{
    return _buffer; 
}

size_t Body::get_size() const 
{
    return _size; 
}

size_t Body::get_max_size() const 
{
    return _maxSize; 
}


void Body::set_size(size_t size) 
{
    _size = size; 
}

void Body::set_max_size(long maxSize) 
{
    _maxSize = maxSize; 
}

void Body::setBuff(std::string const &buffer) 
{
    _buffer = buffer; 
}

void Body::switch_stage(void) {
    stage = !stage;
}

void Body::clear()
{
    _size = 0;
    _buffer.clear();
    stage = SIZE;
}

void Body::recv_buffer_chunk(const std::string& buffer_chunk) {
    if (buffer_chunk.size() != _size) {
	throw StatusLine(400, REASON_400, "Request-Body: bad chunk formatting");
    }
    _buffer.append(buffer_chunk);
    if (_maxSize > 0 && _buffer.size() > _maxSize) {
	throw StatusLine(413, REASON_413, "Request-Body: request-entity exceeds max. body size authorized");
    }
}

void Body::recv_buffer(const std::string& buffer)
{
    _buffer.append(buffer);
    if (_maxSize > 0 && _buffer.size() > _maxSize) {
	throw StatusLine(413, REASON_413, "Request-Body: request-entity exceeds max. body size authorized");
	
    }
    if (_size > 0 && _buffer.size() > _size) {
	throw StatusLine(400, REASON_400, "Request-Body: bad content-length formatting");
    }
}

void swap(Body& a, Body& b)
{
    std::swap(a._size, b._size);
    std::swap(a._maxSize, b._maxSize);
    std::swap(a._buffer, b._buffer);
    std::swap(a.stage, b.stage);
}
