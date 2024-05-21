#!/bin/bash

# sshd setup {for testing}
useradd peter
echo "peter:has&a&password" | chpasswd
service ssh start

# wordpress config setup
sed -i "s/localhost/&:$DB_PORT/"        /var/www/html/wp-config.php;
sed -i "s/database_name_here/$DB_NAME/" /var/www/html/wp-config.php;
sed -i "s/username_here/$DB_USER/"      /var/www/html/wp-config.php;
sed -i "s/password_here/$DB_PASSWORD/"  /var/www/html/wp-config.php;

# mariaDB configuration setup
service mysql start
mysql -u root <<EOF
CREATE DATABASE $DB_NAME;
GRANT ALL ON $DB_NAME.* TO '$DB_USER'@'localhost' IDENTIFIED BY '$DB_PASSWORD';
GRANT ALL ON phpmyadmin.* TO '$PMA_USER'@'localhost' IDENTIFIED BY '$PMA_PASSWD';
GRANt ALL ON $DB_NAME.* TO '$PMA_USER'@'localhost' IDENTIFIED BY '$PMA_PASSWD';
FLUSH PRIVILEGES;
EOF
mysql -u root < /tmp/db.sql

# webserver configuration setup
cat <<EOF > $SERVER_PATH/docker.conf
server {
       listen 8080 ;
       server_name docker-webserver;

       accept_method GET POST ;
       
       index index.php index.html ;

       root /var/www/html/ ;

       location / {
       		cgi_pass php /usr/bin/php-cgi ;
       }
}
EOF

# test
echo "<?php phpinfo(); ?>" > /var/www/html/info.php

# webserver initialization
cd $SERVER_PATH
make
sudo -u www-data ./webserver docker.conf # uncomment when finished testing
echo "Shutting down..."  
  
