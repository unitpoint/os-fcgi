#!/usr/bin/perl
# Call mtr in out-of-source build
$ENV{MTR_BINDIR} = "/tmp/os-igor/ncurses-5.9/mysql/mysql-test";
chdir("/tmp/os-igor/mysql/mysql-test/mysql-test");
exit(system($^X, "/tmp/os-igor/mysql/mysql-test/mysql-test/mysql-test-run.pl", @ARGV) >> 8);
