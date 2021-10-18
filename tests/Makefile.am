## Process this file with automake to produce Makefile.in

TESTS = fileTest
check_PROGRAMS = fileTest
fileTest_SOURCES = utils/fileTest.c $(top_builddir)/src/utils/file.h
fileTest_CFLAGS = @CHECK_CFLAGS@
fileTest_LDADD = $(top_builddir)/src/libmoney.la @CHECK_LIBS@
