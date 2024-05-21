#include "msg_format/StatusLine.hpp"


StatusLine::StatusLine() : _code(-1), _reason() {}

StatusLine::StatusLine(int code, const char* reason, const std::string& addInfos = "") :_code(code), _reason(reason), _addInfos(addInfos) {}

StatusLine::StatusLine(const StatusLine& c) :_code(c._code), _reason(c._reason), _addInfos(c._addInfos) {}

StatusLine::~StatusLine() {}

StatusLine& StatusLine::operator=(StatusLine a)
{
	swap(*this, a);
	return *this;
}

void StatusLine::setCode(int code)
{ 
    _code = code; 
}

void StatusLine::setReason(const std::string& reason)
{ 
    _reason = reason; 
}

void StatusLine::setAdditionalInfo(const std::string& addInfos)
{ 
    _addInfos  = addInfos; 
}

int StatusLine::getCode() const 
{ 
    return _code; 
}

const std::string& StatusLine::getReason() const 
{ 
    return _reason; 
}

const std::string& StatusLine::getAdditionalInfo() const 
{ 
    return _addInfos; 
}

void StatusLine::clear()
{
	_code = -1;
	_reason.clear();
	_addInfos.clear();
}
		
void StatusLine::print() const
{
	std::cout << "------ REQUEST LINE ------\ncode: " << _code << ", reason: |" << _reason
		<< "|, add info: |" << _addInfos << "|\n";
}

void swap(StatusLine& a, StatusLine& b)
{
	std::swap(a._code, b._code);
	std::swap(a._reason, b._reason);
	std::swap(a._addInfos, b._addInfos);
}

inline std::ostream& operator<<(std::ostream& stream, const StatusLine& staLine)
{
	stream << staLine.getCode() << ": " << staLine.getReason();
	
	if (!staLine.getAdditionalInfo().empty())
		stream << ": " << staLine.getAdditionalInfo();
		
	return stream;
}
