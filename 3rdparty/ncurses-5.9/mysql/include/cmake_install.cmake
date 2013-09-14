# Install script for directory: /tmp/os-igor/mysql/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/tmp/os-igor/mysql/include/mysql.h"
    "/tmp/os-igor/mysql/include/mysql_com.h"
    "/tmp/os-igor/mysql/include/mysql_time.h"
    "/tmp/os-igor/mysql/include/my_list.h"
    "/tmp/os-igor/mysql/include/my_alloc.h"
    "/tmp/os-igor/mysql/include/typelib.h"
    "/tmp/os-igor/mysql/include/mysql/plugin.h"
    "/tmp/os-igor/mysql/include/mysql/plugin_audit.h"
    "/tmp/os-igor/mysql/include/mysql/plugin_ftparser.h"
    "/tmp/os-igor/mysql/include/my_dbug.h"
    "/tmp/os-igor/mysql/include/m_string.h"
    "/tmp/os-igor/mysql/include/my_sys.h"
    "/tmp/os-igor/mysql/include/my_xml.h"
    "/tmp/os-igor/mysql/include/mysql_embed.h"
    "/tmp/os-igor/mysql/include/my_pthread.h"
    "/tmp/os-igor/mysql/include/decimal.h"
    "/tmp/os-igor/mysql/include/errmsg.h"
    "/tmp/os-igor/mysql/include/my_global.h"
    "/tmp/os-igor/mysql/include/my_net.h"
    "/tmp/os-igor/mysql/include/my_getopt.h"
    "/tmp/os-igor/mysql/include/sslopt-longopts.h"
    "/tmp/os-igor/mysql/include/my_dir.h"
    "/tmp/os-igor/mysql/include/sslopt-vars.h"
    "/tmp/os-igor/mysql/include/sslopt-case.h"
    "/tmp/os-igor/mysql/include/sql_common.h"
    "/tmp/os-igor/mysql/include/keycache.h"
    "/tmp/os-igor/mysql/include/m_ctype.h"
    "/tmp/os-igor/mysql/include/my_attribute.h"
    "/tmp/os-igor/mysql/include/my_compiler.h"
    "/tmp/os-igor/ncurses-5.9/mysql/include/mysql_version.h"
    "/tmp/os-igor/ncurses-5.9/mysql/include/my_config.h"
    "/tmp/os-igor/ncurses-5.9/mysql/include/mysqld_ername.h"
    "/tmp/os-igor/ncurses-5.9/mysql/include/mysqld_error.h"
    "/tmp/os-igor/ncurses-5.9/mysql/include/sql_state.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mysql" TYPE DIRECTORY FILES "/tmp/os-igor/mysql/include/mysql/" FILES_MATCHING REGEX "/[^/]*\\.h$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

