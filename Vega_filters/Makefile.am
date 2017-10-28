AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xastro_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = Vega_filters

Vega_filters_SOURCES = src/Vega_filter_flux.cpp
Vega_filters_LDADD = ${xastro_LIBS} ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}

