#!/bin/bash

DOC_IMAGE=docker-webserver
DOC_CONT=docker-webserver
SUDO=""

if [ $(uname) = "Linux" ]; then
    SUDO="sudo"
fi

if [ "$1" = "clean" ]; then
    DOC_IMAGE=$($SUDO docker images | grep $DOC_IMAGE | awk '{print $1}')
    DOC_CONT=$($SUDO docker ps -a | grep $DOC_CONT | awk '{print $1}')

    if [ -n "$DOC_CONT" ]; then
	$SUDO docker rm $DOC_CONT
    fi
    if [ -n "$DOC_IMAGE" ]; then
	$SUDO docker rmi $DOC_IMAGE
    fi
    exit 0;
fi

make fclean

if [[ -z $($SUDO docker images | grep $DOC_IMAGE | awk '{print $1}') ]]; then
    $SUDO docker build . -t $DOC_IMAGE
fi

#$SUDO docker run -p 2222:22 -p 8080:8080 --rm --name $DOC_CONT -it $DOC_IMAGE
$SUDO docker run -p 8080:8080 --rm --name $DOC_CONT -it $DOC_IMAGE
