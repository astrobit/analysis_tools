AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${esplot_CFLAGS}  ${radiation_CFLAGS} ${velocity_field_CFLAGS} ${linerout_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS}

bin_PROGRAMS = gaussianprof

gaussianprof_SOURCES = src/gaussian_profile.cpp
gaussianprof_LDADD = ${es_LIBS} ${epsplot_LIBS} ${esplot_LIBS} ${linerout_LIBS} ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}

