AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${epsplot_CFLAGS} ${linerout_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xflash_CFLAGS}

bin_PROGRAMS = quikplotFlash

quikplotFlash_SOURCES = src/quikplot.cpp
quikplotFlash_LDADD = ${es_LIBS} ${epsplot_LIBS} ${linerout_LIBS} ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xflash_LIBS}


