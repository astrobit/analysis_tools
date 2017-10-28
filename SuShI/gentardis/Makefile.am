AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${linerout_CFLAGS} ${xmath_CFLAGS} ${xastro_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = gentardis

gentardis_SOURCES = src/gentardis.cpp
gentardis_LDADD = ${es_LIBS} ${linerout_LIBS} ${xmath_LIBS} ${xastro_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


