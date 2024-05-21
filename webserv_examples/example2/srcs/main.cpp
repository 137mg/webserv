#include "webserv.hpp"
#include "webserver.hpp"

int usage(void) {
  std::cerr << "usage: ./webserver [path_to_config_file | -h]\n";
  return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
    	return usage();
    }
    std::string parser_path("webserver.conf");
    Parser p;
    if (argc == 2) {
      if (!std::string(argv[1]).compare("-h")) {
	return usage();
      }
      parser_path = argv[1];
    }
    try {
        std::vector<server_block_t> srv_t = p.parse(parser_path);
        Webserver websrv(srv_t);

        websrv.run();
    } catch(std::runtime_error& e) {    /* syscall error handler */
        std::cerr << "[ server fatal error ] " <<  e.what() << "\n";
        return EXIT_FAILURE;
    } catch(std::exception& e) {        /* child process error handler (clean exit) */
	std::cerr << "[ cgi error ] " << e.what() << "\n";
	exit(EXECVE_FAIL);
    }
    return EXIT_SUCCESS;
}
