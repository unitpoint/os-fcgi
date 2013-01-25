os-fcgi
=======

ObjectScript language fastcgi daemon

Compile and Install
=======

Run following commands in a *root* shell:

	mkdir build && cd build
	cmake -DCMAKE_INSTALL_PREFIX=/ ..
	make
	make install
	chmod +x /etc/init.d/os-fcgi
	service os-fcgi start