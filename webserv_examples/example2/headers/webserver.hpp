#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include "socket.hpp"
#include "parser.hpp"
#include "request.hpp"
#include <unistd.h>
#include <signal.h>

typedef std::map<std::string, std::string> header_map;

enum socket_status_f {
    CLOSED, /* fin de la conexión */
    STANDBY, /* socket no está aún preparado para realizar acción */
    CONTINUE /* socket preparado */
};

class Webserver {
    private:
        std::vector<Server> server_v;
        std::vector<Socket> read_v;
        std::vector<Socket> write_v;

        struct addr_comp {
            public:
                bool operator()(const Socket&);
                addr_comp(const listen_directive_t&);
                ~addr_comp();
            private:
                const listen_directive_t addr;
        };

        std::string timestamp(void) const;
        void log(const std::string&, const std::string&) const;

        /* parámetros para llamada a select() */
        int nfds;
        fd_set readfds;
        fd_set writefds;

        void check_server_duplicates(const std::vector<Server>&);

        void nfds_up(int);
        void nfds_down(int);

        void accept_new_connection(const Socket&);

        socket_status_f read_from_socket(Socket&);
        socket_status_f write_to_socket(Socket&);

        void ready_to_read_loop(void);
        void ready_to_write_loop(void);

        Webserver(void);
        Webserver(const Webserver&);
        Webserver& operator=(const Webserver&);
    public:
        Webserver(const std::vector<server_block_t>&);
        ~Webserver();

        void run(void);
};

#endif // __WEBSERVER_HPP__
