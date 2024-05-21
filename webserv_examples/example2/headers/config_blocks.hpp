#ifndef __CONFIG_BLOCKS_HPP__
#define __CONFIG_BLOCKS_HPP__
#include <vector>
#include <string>
#include <algorithm>

#define N_DIR_LOC 10
#define N_DIR_SRV 13
#define N_DEFAULT_DIR 5

enum directive_flag_t {
            D_BODY_SIZE,
            D_ROOT,
            D_AUTOINDEX,
            D_INDEX,
            D_METHOD,
	    D_ALIAS,
            D_UPLOAD,
            D_RETURN,
            D_CGI_PASS,
            D_ERROR_PAGE,
            D_LISTEN,
            D_SERVER_NAME,
            D_LOCATION
};

struct location_block_t {
    std::string uri;
    std::vector<std::string> dir[N_DIR_LOC];

    location_block_t(void);
    location_block_t(const std::string&);
    location_block_t(const location_block_t&);
    ~location_block_t();
    
    location_block_t& operator=(const location_block_t&);

    friend bool operator==(const location_block_t&, const std::string&);
    friend bool operator!=(const location_block_t&, const std::string&);
};

struct server_block_t {
    std::vector<location_block_t> loc;
    std::vector<std::string> dir[N_DIR_SRV];

    server_block_t(void);
    server_block_t(const server_block_t&);
    ~server_block_t();
    
    server_block_t& operator=(const server_block_t&);

    void setup_default_directives(void);
    void location_inherits_from_server(location_block_t&);

    operator location_block_t(void) const; 
};

#endif // __CONFIG_BLOCKS_HPP__
