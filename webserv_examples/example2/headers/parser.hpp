#ifndef __PARSER_HPP__
#define __PARSER_HPP__

/* 
 * Formato del archivo de configuración:
 * server {
 *       client_max_body_size -> limita el tamaño de la petición del cliente
 *       root -> indica el inicio de la búsqueda de archivos en el servidor
 *       autoindex -> indica si el servidor ha de producir una lista de directorios cuando
 *                    recibe un directorio como petición
 *       index -> indica los nombres de los archivos a enviar cuando el servidor recibe un
 *                directorio como petición
 *       accept_method -> limita los métodos que el servidor puede aceptar en una ruta concreta
 *       accept_upload -> define la ruta donde el servidor guardará archivos subidos
 *       return -> define a que uri redirige el servidor al acceder a la ruta
 *       cgi_pass -> define la ruta de acceso al cgi, y el tipo de archivos que serán procesados
 *       error_page -> define el archivo que el servidor tratará de enviar tras una solicitud incorrecta
 *       listen -> dirección y puerto del servidor
 *       server_name -> dominio al que responde el servidor
 *       location URI {
 * 
 *       }
 *       location OTHER_URI {
 * 
 *       }       
 * }
 * server { ... }
 *
 * Los bloques de servidor no aceptan rutas con URI idéntica.
 * Tampoco se aceptan varias declaraciones de una misma directiva en un bloque de servidor/ruta
 * No pueden declararse dos bloques de servidor con mismas directivas listen y server_name
 */

#define N_TOK_TYPE 5
#define N_DIR_MAX 13

#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include "config_blocks.hpp"

class Parser {
    public:
        typedef std::string::size_type      size_type;

    private:
        enum token_flag_t {
            T_CBO,
            T_CBC,
            T_SEMICOLON,
            T_SPACE,
            T_WORD,
            T_INVALID_CHAR
        };
        typedef size_t (Parser::*tokenize_options)(int);
        typedef bool (Parser::*directive_parse_table)(void) const;

        enum quotes_flags_e { ON, OFF };
        struct token_t {
            token_flag_t type;
            std::string  token;

            token_t(token_flag_t type_, const std::string& token_);
            token_t(const token_t& other);
        };

        std::deque<token_t> tok_lst;
        std::string         raw_input;

        static const std::string     _dir_name[N_DIR_MAX];
        static directive_parse_table _dir_options[N_DIR_MAX];

        bool    is_number(const std::string& str, int n_max = 0, int n_min = 0) const;
        bool    is_addr(const std::string& addr) const;

        bool    is_word(const token_t& tok) const;
        bool    is_semicolon(const token_t& tok) const;
        bool    is_cbo(const token_t& tok) const;
        bool    is_cbc(const token_t& tok) const;

        bool    syntax_directive_max_body_size(void) const;
        bool    syntax_directive_rruea(void) const; /* root, return, upload, error_page, alias */
        bool    syntax_directive_autoindex(void) const;
        bool    syntax_directive_index(void) const;
        bool    syntax_directive_return(void) const;
        bool    syntax_directive_cgi_pass(void) const;
        bool    syntax_directive_accept_method(void) const;
        bool    syntax_directive_listen(void) const;
        bool    syntax_directive_server_name(void) const;
        bool    syntax_directive_location(void) const;

        directive_flag_t syntax_directive(void);
        location_block_t syntax_location_block(const std::string& uri);
        server_block_t   syntax_server_block(void);

        const token_t& current(void) const;
        bool           empty(void) const;
        token_t        peek(size_type pos = 1) const;
        void           next(void);

        token_flag_t  tokenize_id(char c) const;

        std::string tokenize_word_clean_token(const std::string& token) const;

        size_type  tokenize_curly_bracket(int pos);
        size_type  tokenize_semicolon(int pos);
        size_type  tokenize_word(int pos);
        size_type  tokenize_space(int pos);

        void   read_config_file(const std::string& path);
        void   tokenize(void);
        
        void debug_print(const std::vector<server_block_t>&) const; //tmp
    public:
        Parser(void);
        Parser(const Parser& other);
        ~Parser();

        Parser& operator=(const Parser& other);

        std::vector<server_block_t> parse(const std::string& config_path);
};

#endif // __PARSER_HPP__
