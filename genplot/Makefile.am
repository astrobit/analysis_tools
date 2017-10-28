AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${xio_CFLAGS}  ${xstdlib_CFLAGS}  ${xml2_CFLAGS}

bin_PROGRAMS = genplot

genplot_SOURCES = src/genplot.cpp
genplot_LDADD = ${epsplot_LIBS}  ${xio_LIBS}  ${xstdlib_LIBS} ${xml2_LIBS}

