#include "webserv.hpp"

int ft_write(int fd, std::string wr_buff, size_t wr_buff_s) {
    size_t wr_call_s = 0, wr_ret = 0;
    int    wr_out;
    
    while (wr_buff_s > 0) {
	wr_call_s = (wr_buff_s > S_BUFFR_WR) ?
	    S_BUFFR_WR :
	    wr_buff_s;
	wr_out = write(fd, wr_buff.c_str(), wr_call_s);
	if (wr_out == -1) {
	    return -1;
	}
	wr_ret += wr_out;
	wr_buff.erase(0, wr_call_s);
	wr_buff_s -= wr_call_s;
    }
    return wr_ret;
}
