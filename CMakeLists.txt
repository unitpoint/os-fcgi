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

include_directories(os)
include_directories(fcgi-2.4.1)
include_directories(fcgi-2.4.1/include)
include_directories(MPFDParser-1.0)

find_package(Threads QUIET)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "[ ERROR ]: Could not find thread library.")
endif()

if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_custom_target(strip_${target} ALL COMMAND ${CMAKE_STRIP}
      ${CMAKE_CURRENT_BINARY_DIR}/${target})
endif()

add_executable(${target} ${SOURCES} ${HEADERS})
target_link_libraries(${target} ${CMAKE_THREAD_LIBS_INIT})

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

