# webserv
[ Made with ❤️ by [@danrodri](https://profile.intra.42.fr/users/danrodri/) and [@fgomez-s](https://profile.intra.42.fr/users/fgomez-s) ]

<br/><br/>
<p align="center">
  <i>"I predict the Internet, (...) will soon go <br/>
    spectacularly supernova and in 1996 catastrophically collapse."</i>
</p>
<p align="left">
  <i>~ Robert Metcalfe</i>
</p>
<br/><br/>
Part of 42 common core cursus: develop a simple HTTP-compilant web server, fully compatible with a
web navigator.  
  
This project consists in developing in __C++__ a fully functioning web server, capable to serve at least a fully static website. 
For this, we kept in mind __nginx__ as a reference, to compare header response behavior and configuration file structure.  
Webserv will try to read a configuration file passed as a parameter 
[ or search in a default path if given none ], and if successful, will set up a series of servers 
[ opened sockets for listening to new connections ] with proper config. such as root folder in server host filesystem or a default error page.  
  
  
The main interest on this project is in its core loop; as we are working with just one process without threads, webserv must be implemented using a __multiplexing__ technique to be able to serve fairly to all clients and avoid server hangups.  
The server will work with __non-blocking sockets__ and will use [__select__ ](https://www.man7.org/linux/man-pages/man2/select.2.html) as a multiplexing tool to monitor events in listening and connection sockets.  
  
Lastly, webserv must be fully compliant with the [ __HTTP protocol__ ](https://www.rfc-editor.org/rfc/rfc2616) [ HTTP 1.1 ], and must be compatible with [ __CGI__ ](https://www.rfc-editor.org/rfc/rfc3875) scripts.  
  
## Configuration file
Below there is a configuration file example for webserv.
```nginx

server {
       listen 8080 ;
       server_name server_1;

       root ./html ;
       cgi_pass pl cgi-bin/perl;
       cgi_pass py cgi-bin/python3;

       error_page 404error.html ;
	   
       location /post_test {
       		client_max_body_size 30 ;
		accept_method GET POST HEAD;
       }
       location /cgi_test {
       		accept_method GET POST ;
       }
       location /noticias {
       		return http://www.42madrid.com ;
       }
}

server {
       listen 8989;
       server_name server_2;

       location /cookie_test {
       		accept_method POST GET;
		cgi_pass pl cgi-bin/perl;
       }
}

```
Configuration file options implemented:

* **Listen**  
  Sets up port and host of a virtual server.  
* **error_page**  
  Sets up a default personalized 404 error page option.  
* **Client_max_body_size**  
  Sets payload limit size for a request, will refuse any request bigger that this.  
* **Location**  
  Sets routes for specific configuration inside a virtual server [ as in nginx ].  
* **Accept_method**  
  List of accepted methods [ methods currently implemented in webserv are __HEAD__, __GET__, __POST__, __PUT__ and __DELETE__ ].  
* **Return**  
  Sets up page redirection in location.  
* **Root**  
  Sets up root directory for file searching.  
* **Alias**  
  Sets alias for root directory in a route.  
* **Autoindex**  
  Sets ON | OFF file listing on a directory if an index file is not present.  
* **Index**  
  Sets index page searching for a directory.  
* **Accept_cgi**  
  Sets up list of allowed scripts and path to binary script execution. Must be present file extension of resource and path to script interpreter  
* **Accept_upload**  
  Sets a default upload directory used with a PUT request.  
  
  
## Usage
```
make && ./webserver [path_to_conf_file]
```
  
<br/><br/>
_> Project passed with [125/100] mark ✅😎 <_

   
