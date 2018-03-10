AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${linerout_CFLAGS} ${xmath_CFLAGS} ${xastro_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${es_CFLAGS} ${abundance_CFLAGS}

bin_PROGRAMS = gentardis

gentardis_SOURCES = src/gentardis.cpp
gentardis_LDADD = ${abundance_LIBS} ${xastro_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


