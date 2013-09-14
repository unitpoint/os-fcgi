# Install script for directory: /tmp/os-igor/mysql

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Info")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/docs" TYPE FILE OPTIONAL FILES "/tmp/os-igor/mysql/Docs/mysql.info")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Info")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE OPTIONAL FILES
    "/tmp/os-igor/mysql/COPYING"
    "/tmp/os-igor/mysql/LICENSE.mysql"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/tmp/os-igor/mysql/README")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/docs" TYPE FILE FILES
    "/tmp/os-igor/ncurses-5.9/mysql/Docs/INFO_SRC"
    "/tmp/os-igor/ncurses-5.9/mysql/Docs/INFO_BIN"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "/tmp/os-igor/mysql/Docs/INSTALL-BINARY")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Readme")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Documentation")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/docs" TYPE DIRECTORY FILES "/tmp/os-igor/mysql/Docs/" REGEX "/INSTALL\\-BINARY$" EXCLUDE REGEX "/Makefile\\.[^/]*$" EXCLUDE REGEX "/glibc[^/]*$" EXCLUDE REGEX "/linuxthreads\\.txt$" EXCLUDE REGEX "/myisam\\.txt$" EXCLUDE REGEX "/mysql\\.info$" EXCLUDE REGEX "/sp\\-imp\\-spec\\.txt$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Documentation")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/zlib/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/cmd-line-utils/libedit/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/csv/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/innobase/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/heap/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/myisam/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/federated/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/example/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/archive/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/blackhole/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/perfschema/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/storage/myisammrg/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/plugin/audit_null/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/plugin/auth/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/plugin/fulltext/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/plugin/semisync/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/plugin/daemon_example/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/include/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/dbug/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/strings/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/vio/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/regex/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/mysys/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/libmysql/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/unittest/mytap/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/unittest/mysys/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/extra/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/tests/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/client/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/sql/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/sql/share/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/libservices/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/mysql-test/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/mysql-test/lib/My/SafeProcess/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/support-files/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/scripts/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/sql-bench/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/man/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/packaging/rpm-uln/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/packaging/WiX/cmake_install.cmake")
  INCLUDE("/tmp/os-igor/ncurses-5.9/mysql/packaging/solaris/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

