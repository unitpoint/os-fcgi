os-fcgi
=======

ObjectScript language fastcgi daemon.

Compile and Install
=======

Run following commands in a *root* shell

# native build
	mkdir build && cd build
	cmake -DCMAKE_INSTALL_PREFIX=/ ..
	make
	make install
	service os-fcgi restart

# build 32 bit version on 64 bit server (32 bit version is faster than 64 bit)
	mkdir build && cd build
	cmake -DFORCE32=ON -DCMAKE_INSTALL_PREFIX=/ ..
	make
	make install
	service os-fcgi restart

# dependencies to build the 32 bit version
	libstdc++
	gcc-multilib
	g++-multilib
	bison
	ia32-libs
	
Information
===========

Current os-fcgi version uses 8 threads and listens 127.0.0.1:9000

os-fcgi config is not used at the moment. You could view a demo usage at http://objectscript.org or checkout repo https://github.com/unitpoint/objectscript.org

Nginx config example
====================

	server {
		listen			80;
		server_name		mydomain.com www.mydomain.com;
		root			/home/myuser/mydomain.com/www;
		error_log		/var/log/nginx/error.mydomain.com.log;
		access_log		off;
		location ~ /\.ht {
			deny all;
		}
		location ~ /\.git {
			deny all;
		}
		location / {
			try_files $uri $uri/ /index.osh /index.os;
		}
		location ~* \.(jpg|jpeg|png|gif|swf|flv|mp4|mov|avi|wmv|m4v|mkv|ico|js|css|txt)$ {
			access_log off;
			expires 7d;
		}
		charset	utf-8;
		location ~ ^.+\.osh? {
			fastcgi_split_path_info	^(.+\.osh?)(.*)$;
			fastcgi_pass	127.0.0.1:9000;
			fastcgi_index	index.osh;
			include fastcgi_params;
			fastcgi_intercept_errors	on;
			fastcgi_ignore_client_abort	on;
			fastcgi_read_timeout	360;
		}
	}

Apache config example
=====================

	<VirtualHost mydomain.com:80>
		ServerAdmin webmaster@mydomain.com
		DocumentRoot "/home/myuser/mydomain.com/www"
		ServerName mydomain.com
		
		FastCgiExternalServer "/home/myuser/mydomain.com/www" -host 127.0.0.1:9000
		
		<Directory "/home/myuser/mydomain.com/www">
			# SetHandler fastcgi-script
			AddHandler fastcgi-script .osh
			AddHandler fastcgi-script .os
			Options Indexes FollowSymLinks MultiViews ExecCGI
			AllowOverride all
			Order Deny,Allow
			Deny from all
			Allow from 127.0.0.1
		</Directory>
	</VirtualHost>
