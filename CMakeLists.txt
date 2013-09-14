cmake_minimum_required (VERSION 2.8)

set(target "os-fcgi")
project(${target})

set(INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/_install)

OPTION(FORCE32 "Force a 32bit compile on 64bit" OFF)
IF(FORCE32)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

    link_directories(/lib32)
    link_directories(/usr/lib32)

    include_directories(${INSTALL_DIR}/include)
    link_directories(${INSTALL_DIR}/lib)

    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32 -ldl -L/lib32 -L/usr/lib32")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32 -ldl -L/lib32 -L/usr/lib32")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -ldl -L/lib32 -L/usr/lib32")

    if(CMAKE_SIZEOF_VOID_P MATCHES "8" AND NOT(FORCE32) )
        set(BIN_LIBROOT "linux64")
        set(BIN_RPATH "\$ORIGIN/lib64")
    else()
        set(BIN_LIBROOT "linux")
        set(BIN_RPATH "\$ORIGIN/lib32")
    endif()

    set(CMAKE_SKIP_BUILD_RPATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH ${BIN_RPATH})
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

    set(CMAKE_FIND_ROOT_PATH ${INSTALL_DIR})
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
ENDIF()

IF (FORCE32)
	include(ExternalProject)
	ExternalProject_Add(ncurses
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ncurses-5.9
		CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ncurses-5.9/configure --with-shared --prefix=${INSTALL_DIR} CFLAGS=-m32 CXXFLAGS=-m32
		BUILD_COMMAND make -j8
	)

	ExternalProject_Add(mysql
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mysql-5.5.32
		CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -m32 
				   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -m32 
				   -DWITH_LIBEDIT=ON 
				   -DCURSES_LIBRARY=${INSTALL_DIR}/lib/libncurses.so 
				   -DCURSES_INCLUDE_PATH=${INSTALL_DIR}/include 
				   -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
		BUILD_COMMAND make -j8
	)

	ExternalProject_Add(soci
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/soci-3.2.1/src
		CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -m32 
				   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -m32 
				   -DMYSQL_LIBRARIES=${INSTALL_DIR}/lib/libmysqlclient_r.so 
				   -DMYSQL_INCLUDE_DIR=${INSTALL_DIR}/include 
				   -DWITH_BOOST=OFF 
				   -DWITH_MYSQL=ON 
				   -DSOCI_STATIC=OFF 
				   -DWITH_ODBC=OFF 
				   -DWITHSQLITE3=OFF 
				   -DWITHORACLE=OFF 
				   -DWITHDB2=OFF
				   -DWITH_FIREBIRD=OFF 
				   -DWITH_POSTGRESQL=OFF 
				   -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
		BUILD_COMMAND make -j8 lib
	)

	ExternalProject_Add(curl
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/curl-7.32.0
		CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -m32 
				   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -m32 
				   -DCMAKE_USE_OPENSSL=OFF 
				   -DBUILD_CURL_EXE=OFF 
				   -DBUILD_CURL_TESTS=OFF 
				   -DCURL_ZLIB=OFF 
				   -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
		BUILD_COMMAND make -j8
	)

	ExternalProject_Add(pcre
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pcre-8.33 
		CMAKE_ARGS -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} -m32 
				   -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} -m32 
				   -DPCRE_SUPPORT_LIBZ=OFF 
				   -DPCRE_SUPPORT_LIBREADLINE=OFF 
				   -DBUILD_SHARED_LIBS=ON 
				   -DPCRE_SUPPORT_UTF=ON 
				   -DPCRE_SUPPORT_UNICODE_PROPERTIES=ON 
				   -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
		BUILD_COMMAND make -j8
	)
ENDIF()

list(APPEND SOURCES os-fcgi.cpp)
list(APPEND SOURCES os/objectscript.cpp)
list(APPEND SOURCES 3rdparty/fcgi-2.4.1/libfcgi/os_unix.c)
list(APPEND SOURCES 3rdparty/fcgi-2.4.1/libfcgi/fcgiapp.c)
list(APPEND SOURCES 3rdparty/MPFDParser-1.0/Parser.cpp)
list(APPEND SOURCES 3rdparty/MPFDParser-1.0/Field.cpp)
list(APPEND SOURCES 3rdparty/MPFDParser-1.0/Exception.cpp)
list(APPEND SOURCES 3rdparty/md5/md5.cpp)
list(APPEND SOURCES os/ext-odbo/os-odbo.cpp)

include_directories(os)
include_directories(3rdparty/fcgi-2.4.1)
include_directories(3rdparty/fcgi-2.4.1/include)
include_directories(3rdparty/MPFDParser-1.0)
include_directories(3rdparty/md5)


find_package(Threads QUIET)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "[ ERROR ]: Could not find thread library.")
endif()

IF (NOT FORCE32)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
    find_package(PCRE QUIET)
    if(PCRE_FOUND)
		include_directories(${PCRE_INCLUDE_DIR})
		list(APPEND SOURCES os/ext-regexp/os-regexp.cpp)
		list(APPEND HEADERS os/ext-regexp/os-regexp.h)
    else()
		message(STATUS "[ WARNING ]: Could not find PCRE library")
		add_definitions( -DOS_REGEXP_DISABLED )
    endif()
ELSE()
    list(APPEND SOURCES os/ext-regexp/os-regexp.cpp)
    list(APPEND HEADERS os/ext-regexp/os-regexp.h)
    set(PCRE_LIBRARIES ${INSTALL_DIR}/lib/libpcrecpp.so ${INSTALL_DIR}/lib/libpcreposix.so ${INSTALL_DIR}/lib/libpcre.so)
ENDIF()

IF (NOT FORCE32)
    find_package(MySQL QUIET)
    if(MYSQL_FOUND)
        include_directories(${MYSQL_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "[ERROR]: Couldn't find MySQL include files and/or library.")
    endif()
ELSE()
    include_directories(${INSTALL_DIR}/include/mysql)
    set(MYSQL_LIBRARIES ${INSTALL_DIR}/lib/libmysqlclient_r.so)
ENDIF()

IF (NOT FORCE32)
    find_package(Soci QUIET)
    if(SOCI_FOUND)
        include_directories(${SOCI_INCLUDE_DIR})
		include_directories(${SOCI_INCLUDE_DIR}/mysql)
    else()
        message(FATAL_ERROR "[ERROR]: Couldn't find Soci include files and/or library.")
    endif()
ELSE()
    include_directories(${INSTALL_DIR}/include/soci)
    include_directories(${INSTALL_DIR}/include/soci/mysql)
    set(SOCI_LIBRARY ${INSTALL_DIR}/lib/libsoci_core.so)
    set(SOCI_mysql_PLUGIN ${INSTALL_DIR}/lib/libsoci_mysql.so)
ENDIF()

IF (NOT FORCE32)
    find_package(CURL QUIET)
    if(CURL_FOUND)
    	include_directories(${CMAKE_CURRENT_BINARY_DIR}/3rdparty/curl-7.32.0/include)
		list(APPEND SOURCES os/ext-curl/os-curl.cpp)
		list(APPEND HEADERS os/ext-curl/os-curl.h)
    else()
    	message(STATUS "[ WARNING ]: Could not find cURL")
		add_definitions( -DOS_CURL_DISABLED )
    endif()
ELSE()
    include_directories(${INSTALL_DIR}/include/curl)
    list(APPEND SOURCES os/ext-curl/os-curl.cpp)
    list(APPEND HEADERS os/ext-curl/os-curl.h)
    set(CURL_LIBRARIES ${INSTALL_DIR}/lib/libcurl.so)
ENDIF()

# begin sqlite3
list(APPEND SOURCES os/ext-sqlite3/sqlite3.c)
list(APPEND HEADERS os/ext-sqlite3/sqlite3.h)
list(APPEND SOURCES os/ext-sqlite3/os-sqlite3.cpp)
list(APPEND HEADERS os/ext-sqlite3/os-sqlite3.h)

SET(GCC_COVERAGE_COMPILE_FLAGS "${GCC_COVERAGE_COMPILE_FLAGS} ")
SET(GCC_COVERAGE_LINK_FLAGS    "${GCC_COVERAGE_LINK_FLAGS} -lpthread -ldl")
# end sqlite3

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_custom_target(strip_${target} ALL COMMAND ${CMAKE_STRIP}
      ${CMAKE_CURRENT_BINARY_DIR}/${target})
endif()


add_executable(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} ${PCRE_LIBRARIES} ${MYSQL_LIBRARIES} ${SOCI_LIBRARY} ${SOCI_mysql_PLUGIN} dl)

IF (FORCE32)
    add_dependencies(mysql ncurses)
    add_dependencies(soci mysql)
    add_dependencies(${target} pcre curl soci)
ENDIF()


install(TARGETS ${target} RUNTIME DESTINATION usr/sbin)
install(FILES conf/etc/init.d/${target} DESTINATION etc/init.d
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                GROUP_EXECUTE GROUP_READ
                WORLD_READ WORLD_EXECUTE)

install(CODE "
    if(NOT EXISTS ${CMAKE_INSTALL_PREFIX}/etc/${target}/conf.os)
        execute_process(COMMAND ${CMAKE_COMMAND} -E
            copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/conf/etc/${target} ${CMAKE_INSTALL_PREFIX}/etc/${target})
    endif()"
)

IF (FORCE32)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${INSTALL_DIR}/lib ${CMAKE_INSTALL_PREFIX}/usr/lib32)")
ENDIF()

