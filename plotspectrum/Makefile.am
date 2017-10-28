AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${esplot_CFLAGS} ${linerout_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS}  ${xstdlib_CFLAGS} ${es_CFLAGS}

bin_PROGRAMS = plotspec

plotspec_SOURCES = src/quikplotspectrum.cpp
plotspec_LDADD = ${es_LIBS} ${esplot_LIBS} ${epsplot_LIBS} ${linerout_LIBS} ${xmath_LIBS} ${xio_LIBS}  ${xstdlib_LIBS}

