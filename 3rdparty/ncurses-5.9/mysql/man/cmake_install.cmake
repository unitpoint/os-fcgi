# Install script for directory: /tmp/os-igor/mysql/man

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "ManPages")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES
    "/tmp/os-igor/mysql/man/mysqladmin.1"
    "/tmp/os-igor/mysql/man/comp_err.1"
    "/tmp/os-igor/mysql/man/mysql_plugin.1"
    "/tmp/os-igor/mysql/man/msql2mysql.1"
    "/tmp/os-igor/mysql/man/innochecksum.1"
    "/tmp/os-igor/mysql/man/mysqld_multi.1"
    "/tmp/os-igor/mysql/man/mysql_client_test_embedded.1"
    "/tmp/os-igor/mysql/man/mysql_client_test.1"
    "/tmp/os-igor/mysql/man/mysql-stress-test.pl.1"
    "/tmp/os-igor/mysql/man/mysqlbug.1"
    "/tmp/os-igor/mysql/man/mysql_fix_extensions.1"
    "/tmp/os-igor/mysql/man/resolveip.1"
    "/tmp/os-igor/mysql/man/mysqltest_embedded.1"
    "/tmp/os-igor/mysql/man/mysql_setpermission.1"
    "/tmp/os-igor/mysql/man/mysql_tzinfo_to_sql.1"
    "/tmp/os-igor/mysql/man/mysql_zap.1"
    "/tmp/os-igor/mysql/man/mysql_convert_table_format.1"
    "/tmp/os-igor/mysql/man/my_print_defaults.1"
    "/tmp/os-igor/mysql/man/mysqlshow.1"
    "/tmp/os-igor/mysql/man/myisam_ftdump.1"
    "/tmp/os-igor/mysql/man/myisampack.1"
    "/tmp/os-igor/mysql/man/replace.1"
    "/tmp/os-igor/mysql/man/mysqlbinlog.1"
    "/tmp/os-igor/mysql/man/mysql_upgrade.1"
    "/tmp/os-igor/mysql/man/perror.1"
    "/tmp/os-igor/mysql/man/mysql_config.1"
    "/tmp/os-igor/mysql/man/mysqlhotcopy.1"
    "/tmp/os-igor/mysql/man/mysql_find_rows.1"
    "/tmp/os-igor/mysql/man/mysql.server.1"
    "/tmp/os-igor/mysql/man/mysql.1"
    "/tmp/os-igor/mysql/man/mysqltest.1"
    "/tmp/os-igor/mysql/man/mysql_waitpid.1"
    "/tmp/os-igor/mysql/man/mysqldump.1"
    "/tmp/os-igor/mysql/man/mysqldumpslow.1"
    "/tmp/os-igor/mysql/man/mysql_secure_installation.1"
    "/tmp/os-igor/mysql/man/mysqlman.1"
    "/tmp/os-igor/mysql/man/mysql_install_db.1"
    "/tmp/os-igor/mysql/man/resolve_stack_dump.1"
    "/tmp/os-igor/mysql/man/mysqlslap.1"
    "/tmp/os-igor/mysql/man/myisamlog.1"
    "/tmp/os-igor/mysql/man/myisamchk.1"
    "/tmp/os-igor/mysql/man/mysql-test-run.pl.1"
    "/tmp/os-igor/mysql/man/mysqld_safe.1"
    "/tmp/os-igor/mysql/man/mysqlaccess.1"
    "/tmp/os-igor/mysql/man/mysqlcheck.1"
    "/tmp/os-igor/mysql/man/mysqlimport.1"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "ManPages")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "ManPages")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man8" TYPE FILE FILES "/tmp/os-igor/mysql/man/mysqld.8")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "ManPages")

