AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${linerout_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS}

bin_PROGRAMS = equivwidth

equivwidth_SOURCES = src/equivalentwidth.cpp
equivwidth_LDADD =  ${linerout_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xmath_LIBS} ${es_LIBS}

