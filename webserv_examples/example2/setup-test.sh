#!/bin/bash

TESTER=tester
CGI_PATH=cgi-bin

if [ "$1" = "clean" ]; then
    rm -rfv YoupiBanane cgi-bin
    rm -v $TESTER
    rm -v webs.conf
    make fclean
    exit
fi

if [ ! -d "./YoupiBanane" ]; then 
    mkdir -pv {cgi-bin,YoupiBanane/{nop,Yeah,put_test,upload_files,post_body}}
    touch YoupiBanane/youpi.bad_extension \
	  YoupiBanane/youpi.bla \
	  YoupiBanane/nop/youpi.bad_extension \
	  YoupiBanane/nop/other.pouic \
	  YoupiBanane/Yeah/not_happy.bad_extension
    if [ $(uname) == "Linux" ]; then
	wget -O $PWD/$CGI_PATH/cgi_tester https://projects.intra.42.fr/uploads/document/document/6716/ubuntu_cgi_tester
    else
	wget -O $PWD/$CGI_PATH/cgi_tester https://projects.intra.42.fr/uploads/document/document/6717/cgi_tester

    fi
    chmod +x $CGI_PATH/cgi_tester
fi

if [ ! -f $TESTER ]; then
    if [ $(uname) == "Linux" ]; then
	wget -O $PWD/$TESTER https://projects.intra.42.fr/uploads/document/document/6718/ubuntu_tester
    else
	wget -O $PWD/$TESTER https://projects.intra.42.fr/uploads/document/document/6715/tester
    fi
    chmod +x $TESTER
fi

if [ ! -f webs.conf ]; then
    cat <<EOF > webs.conf
server {
       listen 8080 ;

       accept_method GET POST;
       cgi_pass bla $PWD/cgi-bin/cgi_tester ;
       location / {
       		accept_method GET ;
       }
       location /put_test/ {
       		root ./YoupiBanane/ ;
                accept_method PUT ;
                accept_upload /upload_files/ ;
       }
       location /post_body {
       		root ./YoupiBanane/ ;
                accept_method POST ;
                client_max_body_size 100 ;
       }
       location /directory {
                index youpi.bad_extension ;
                alias ./YoupiBanane/ ;
       }
}
EOF
fi

make
echo "Ready: ./webserver webs.conf && ./$TESTER http://localhost:8080"
