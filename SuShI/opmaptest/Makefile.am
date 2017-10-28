AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${linerout_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = opmaptest

opmaptest_SOURCES = src/opacity_map_test.cpp
opmaptest_LDADD = ${linerout_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


