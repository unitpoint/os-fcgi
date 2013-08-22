cmake_minimum_required (VERSION 2.8)

set(target "os-fcgi")
project(${target})

list(APPEND SOURCES os-fcgi.cpp)
list(APPEND SOURCES os/objectscript.cpp)
list(APPEND SOURCES fcgi-2.4.1/libfcgi/os_unix.c)
list(APPEND SOURCES fcgi-2.4.1/libfcgi/fcgiapp.c)
list(APPEND SOURCES MPFDParser-1.0/Parser.cpp)
list(APPEND SOURCES MPFDParser-1.0/Field.cpp)
list(APPEND SOURCES MPFDParser-1.0/Exception.cpp)
list(APPEND SOURCES md5/md5.cpp)
list(APPEND SOURCES os/ext-odbo/os-odbo.cpp)


include_directories(os)
include_directories(fcgi-2.4.1)
include_directories(fcgi-2.4.1/include)
include_directories(MPFDParser-1.0)
include_directories(md5)
include_directories(soci/include)
include_directories(soci/include/mysql)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/soci/lib)

find_package(Threads QUIET)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "[ ERROR ]: Could not find thread library.")
endif()

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

find_package(MySQL QUIET)
if(MYSQL_FOUND)
    include_directories(${MYSQL_INCLUDE_DIR})
else()
    message(FATAL_ERROR "[ERROR]: Couldn't find MySQL include files and/or library.")
endif()

find_package(CURL QUIET)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
	list(APPEND SOURCES os/ext-curl/os-curl.cpp)
	list(APPEND HEADERS os/ext-curl/os-curl.h)
else()
    message(STATUS "[ WARNING ]: Could not find cURL")
	add_definitions( -DOS_CURL_DISABLED )
endif()

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

message(STATUS "MYSQL_LIBRARIES = ${MYSQL_LIBRARIES}")

add_executable(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} ${PCRE_LIBRARIES} ${MYSQL_LIBRARIES} soci_core soci_mysql dl)

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

