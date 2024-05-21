FROM debian:buster

ENV SERVER_PATH="/var/www"

ENV DB_NAME="wp_database"
ENV DB_USER="wp_user"
ENV DB_PASSWORD="wp_passwd"
ENV DB_PORT="8080"

ENV PMA_USER="pma_admin"
ENV PMA_PASSWD="pma_passwd"

ADD [".", "/var/www/"]

COPY ["./docker-utils/docker-entrypoint.sh", "/tmp"]
COPY ["./docker-utils/db.sql", "/tmp"]

RUN apt-get update \
    && apt-get install wget \
       	       	   sudo \
       	       	   curl \
                   make \
                   clang \
		   ssh \
                   default-mysql-server \
                   php \
		   php-curl \
		   php-dom \
		   php-exif \
		   php-fileinfo \
		   php-imagick \
		   php-json \
		   php-mbstring \
		   php-xml \
		   php-zip \
                   php-cgi \
                   php-mysql -y \
    &&  cd /tmp \               
    &&  wget -c https://www.wordpress.org/latest.tar.gz \
    &&  tar -xvf latest.tar.gz \
    &&  mkdir -pv $SERVER_PATH/html \
    &&  mv -v wordpress/* $SERVER_PATH/html/ \
    &&  mv $SERVER_PATH/html/wp-config-sample.php $SERVER_PATH/html/wp-config.php \
    &&  wget -c \
    	https://files.phpmyadmin.net/phpMyAdmin/5.0.4/phpMyAdmin-5.0.4-all-languages.tar.gz \
    &&  tar -xvf phpMyAdmin-5.0.4-all-languages.tar.gz \    
    &&  mv phpMyAdmin-5.0.4-all-languages $SERVER_PATH/html/phpmyadmin \
    &&  rm -rfv /tmp/{wordpress/,*.tar.gz} \
    &&  chown -R www-data:www-data $SERVER_PATH \
    &&  chmod -R 755 $SERVER_PATH \
    &&  chmod +x ./docker-entrypoint.sh

COPY --chown=www-data:www-data ["./docker-utils/wp-config.php",  "$SERVER_PATH/html/phpmyadmin/"]

EXPOSE 8080/tcp
EXPOSE 22/tcp

WORKDIR "/var/www"

ENTRYPOINT [ "/tmp/docker-entrypoint.sh" ]
