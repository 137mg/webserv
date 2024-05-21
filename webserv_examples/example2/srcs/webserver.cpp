#include "webserver.hpp"

sig_atomic_t    quit_f = 0;
static void sighandl(int signal) {
    if (signal == SIGINT || signal == SIGQUIT) {
        quit_f = 1;
    }
}

bool    Webserver::addr_comp::operator()(const Socket& other) {
    return addr == other.get_socket_addr();
}

Webserver::addr_comp::addr_comp(const listen_directive_t& _addr) : addr(_addr) { }

Webserver::addr_comp::~addr_comp() { }

/* condición de server duplicado: comparten mismas directivas listen y server_name */
void    Webserver::check_server_duplicates(const std::vector<Server>& srv_v) {
    if (srv_v.empty()) {
	throw std::runtime_error("no server defined");
    }
    for (std::vector<Server>::const_iterator it = srv_v.begin(); it != --srv_v.end(); it++) {
        for (std::vector<Server>::const_iterator it_n = it + 1; it_n != srv_v.end(); it_n++) {
            if (*it_n == *it) {
                throw std::runtime_error("duplicate server error");
            }
        }
    }
}

std::string    Webserver::timestamp(void) const {
    std::time_t t = std::time(0);
    std::tm*    time = std::localtime(&t);
    std::string format_time(asctime(time));

    format_time.erase(format_time.size() - 1);
    return format_time;
}

void    Webserver::log(const std::string& action, const std::string& error) const {
    std::cerr << "[ " << timestamp() << " ] " << action << error << "\n";
}

void    Webserver::nfds_up(int fd) {
    if (fd + 1 >= nfds) {
        nfds = fd + 1;
    }
}

void    Webserver::nfds_down(int fd) {
    if (fd + 1 == nfds) {
        --nfds;
    }
}

/* acepta nuevas conexiones de los sockets pasivos; no trata fallos en accept como críticos */
void    Webserver::accept_new_connection(const Socket& passv) {
    struct sockaddr_in client_addr;
    socklen_t          client_addr_len = sizeof(client_addr);
    int                client_fd = accept(passv.fd,
					  reinterpret_cast<sa_t*>(&client_addr),
					  &client_addr_len);
    
    if (client_fd == -1) {
        log ("accept(): ", strerror(errno));
        return ;
    }
    
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
	log ("fcntl(): ", strerror(errno));
	return ;
    }
    std::pair<std::string, std::string> client_info(
	inet_ntoa(client_addr.sin_addr),
	n_to_str(ntohs(client_addr.sin_port))
    );
    Socket sck_new(client_fd, passv, client_info);

    read_v.push_back(Socket(client_fd, passv, client_info));
    nfds_up(read_v.back().fd);
}

/* lectura de socket activo; parsea request y construye respuesta */
socket_status_f    Webserver::read_from_socket(Socket& conn_socket) {
    char     req_buff[REQUEST_BUFFER_SIZE] = {0};
//    memset(req_buff, 0, REQUEST_BUFFER_SIZE);
    int rd_ret = recv(conn_socket.fd, req_buff, REQUEST_BUFFER_SIZE, 0);
    
    if (rd_ret == -1) {
        log("read(): ", std::strerror(errno));
    }
    if (rd_ret <= 0) {
        conn_socket.close_socket();
        nfds_down(conn_socket.fd);
        return CLOSED;
    }
    try {
        conn_socket.build_request(req_buff, rd_ret);
    } catch (StatusLine& sl) {
	log("request: ", n_to_str(sl.getCode()) + " " + sl.getReason() + " - " + sl.getAdditionalInfo());
        conn_socket.build_response(sl);
        return CONTINUE;
    }
    return STANDBY;
}

/* llamada a write con el mensaje guardado en el Socket */
socket_status_f Webserver::write_to_socket(Socket& conn_socket) {
    const std::string& response = conn_socket.get_response_string();
    int                wr_ret = ft_write(conn_socket.fd, response, response.size());
    
    if (wr_ret <= 0 || conn_socket.marked_for_closing()) {
        conn_socket.close_socket();
        return CLOSED;
    }
    conn_socket.clear_response();
    return CONTINUE;
}

void Webserver::ready_to_read_loop(void) {
    for (size_t i = 0; i < read_v.size(); i++) {
        if (FD_ISSET(read_v[i].fd, &readfds)){
            if (read_v[i].is_passv()) {
                accept_new_connection(read_v[i]);
                continue ;
            }
            socket_status_f conn_stat = read_from_socket(read_v[i]);

            if (conn_stat == CONTINUE) {
                write_v.push_back(read_v[i]);
            }
            if (conn_stat != STANDBY) {
                read_v.erase(read_v.begin() + i);
            }
        }
    }
}

void Webserver::ready_to_write_loop(void) {
    for (size_t i = 0; i < write_v.size(); i++) {
        if (FD_ISSET(write_v[i].fd, &writefds)) {
            socket_status_f conn_stat = write_to_socket(write_v[i]);

            if (conn_stat == CONTINUE) {
                read_v.push_back(write_v[i]);
            }
            if (conn_stat != STANDBY) {
                write_v.erase(write_v.begin() + i);
            }
        }
    }
}

Webserver::Webserver(void) : nfds(0) { }

Webserver::Webserver(const Webserver& other) { 
    *this = other;
}

Webserver& Webserver::operator=(const Webserver& other) { 
    if (this == &other) {
        return *this;
    }
    server_v = other.server_v;
    read_v = other.read_v;
    write_v = other.write_v;
    return *this;
}

/* inicializa servidores y sockets pasivos en base a las estructuras generadas por el parser */
Webserver::Webserver(const std::vector<server_block_t>& srv_blk_v) : nfds(0) {
    for (std::vector<server_block_t>::const_iterator it = srv_blk_v.begin(); it != srv_blk_v.end(); it++) {
        server_v.push_back(*it);
    }
    check_server_duplicates(server_v);
    for (std::vector<Server>::const_iterator it = server_v.begin(); it != server_v.end(); it++) {
        Webserver::addr_comp comp(it->get_server_addr());
        std::vector<Socket>::iterator sock_it = std::find_if(read_v.begin(), read_v.end(), comp);

        if (sock_it == read_v.end()) {
            read_v.push_back(it->get_server_addr());
            read_v.back().add_server_ref(*it);
            nfds_up(read_v.back().fd);
        } else {
            sock_it->add_server_ref(*it);
        }
    }
}

Webserver::~Webserver() { 
    for (std::vector<Socket>::iterator it = read_v.begin(); it != read_v.end(); it++) {
        it->close_socket();
    }
    for (std::vector<Socket>::iterator it = write_v.begin(); it != write_v.end(); it++) {
        it->close_socket();
    }
}

void Webserver::run(void) {
    signal(SIGINT, &sighandl);
    signal(SIGQUIT, &sighandl);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    log("\033[32m[ webserver is up and running.", " ~ /danrodri, /fgomez-s ]\033[0m");
    while (!quit_f) {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);

        for (std::vector<Socket>::iterator it = read_v.begin(); it != read_v.end(); it++) {
            FD_SET(it->fd, &readfds);
        }
        for (std::vector<Socket>::iterator it = write_v.begin(); it != write_v.end(); it++) {
            FD_SET(it->fd, &writefds);
        }
        if ((select(nfds, &readfds, &writefds, NULL, NULL)) == -1) {
            throw std::runtime_error(strerror(errno));
        }
        ready_to_read_loop();
        ready_to_write_loop();
    }
}
