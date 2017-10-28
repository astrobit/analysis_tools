AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xmath_CFLAGS} ${xio_CFLAGS}  ${xstdlib_CFLAGS}

bin_PROGRAMS = linfit

linfit_SOURCES = src/linfit.cpp
linfit_LDADD = ${xmath_LIBS} ${xio_LIBS}  ${xstdlib_LIBS}

