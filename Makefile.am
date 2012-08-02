# Process this file with automake to create Makefile.in

ACLOCAL_AMFLAGS = -I m4

#lib_LIBRARIES      = libquickmail.a
#libquickmail_a_SOURCES = quickmail.c

lib_LTLIBRARIES     = libquickmail.la
libquickmail_la_SOURCES = quickmail.c
libquickmail_la_LDFLAGS = -Wl,-no-undefined
libquickmail_la_HEADERS = quickmail.h
libquickmail_ladir = ${includedir}

check_PROGRAMS = test_quickmail
test_quickmail_SOURCES = test_quickmail.c
test_quickmail_LDADD = libquickmail.la

TESTS = test_quickmail
