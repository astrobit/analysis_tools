AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${epsplot_CFLAGS} ${linerout_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${msdb_CFLAGS}

bin_PROGRAMS = modelvelev

modelvelev_SOURCES = src/model_vel_evolution.cpp
modelvelev_LDADD = ${es_LIBS} ${epsplot_LIBS} ${linerout_LIBS} ${xastro_LIBS} ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${msdb_LIBS}


