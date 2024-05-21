#ifndef BODY_HPP
#define BODY_HPP

#include "../webserv.hpp"
#include "StatusLine.hpp"

#define SIZE  0
#define CHUNK 1

class Body
{
private:
    size_t      _size;		// Content-length size
    size_t      _maxSize;	// Max octets that Body object can receive
    std::string	_buffer;		// Buffer containing the payload

public:
    Body();
    Body(const Body& c);
    ~Body();
    Body& operator=(Body a);

    int stage;
    /* --------------------------- GETTERS ------------------------- */

    const std::string& get_body(void) const ;
    size_t             get_size(void) const ;
    size_t             get_max_size(void) const ;


    /* --------------------------- SETTERS ------------------------- */
		
    void set_size(size_t size);
    void set_max_size(long maxSize);
    void setBuff(std::string const &buf);

    void switch_stage(void);
    /* --------------------------- METHODS ------------------------- */

    // Returns true or false depending if request line + all headers have been received
    //  bool isReceiving() const;
		
    // Reset the Body object
    void clear();
		
    // Append buffer received from client until content-length octets have been received.
    void recv_buffer_chunk(const std::string&);
    void recv_buffer(const std::string&);
		
	
    /* --------------- NON-MEMBER FUNCTION OVERLOADS --------------- */

    friend void swap(Body& a, Body& b);
}; 

#endif
