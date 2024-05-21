#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "location.hpp"

struct listen_directive_t {
    std::string addr;
    id_t        port;

    listen_directive_t(void);
    listen_directive_t(const listen_directive_t&);
    listen_directive_t(const std::string&);
    ~listen_directive_t();
    listen_directive_t& operator=(const listen_directive_t&);

    bool operator==(const listen_directive_t&) const;
    bool operator!=(const listen_directive_t&) const;
};

class Server {
    private:
        listen_directive_t       listen; /* dirección y puerto en los que escucha el server */
        std::vector<Location>    routes; /* bloques de ruta dentro del servidor */

        std::string get_uri_from_request(const std::string&) const;
    public:
        Server(void);
        Server(const Server&);
        Server(const server_block_t&);
        ~Server();

        std::vector<std::string> server_name;

        const listen_directive_t get_server_addr(void) const;

        const Location& get_location_by_path(const std::string&) const;

        bool operator==(const Server&) const;
        bool operator!=(const Server&) const;
};

typedef std::vector<const Server*> server_vector;

class find_server_by_host {
    public:
        find_server_by_host(const std::string&);
        bool operator() (const Server*);

    private:
        std::string hostname;
};

#endif // __SERVER_HPP__
