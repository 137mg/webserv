#include "Request.hpp"
#include <cstring>

Request::Request()
{
	clear();
}

void Request::clear(void)
{
	Message::clear();

	file_fd = -1;
	file_size = 0;
	RW_file_size = 0;
	writeIndex = 0;
	is_static = true;
	file_name.clear();
	path.clear();
	cgi_args.clear();
	cgi_path_info.clear();
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	pipe_fd_back[0] = -1;
	pipe_fd_back[1] = -1;
	location_uri.clear();
	uri.clear();
	errorCode = NOT_ERROR;
	is_error = false;
}
