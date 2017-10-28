AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${top_srcdir}/SuShI/include ${epsplot_CFLAGS} ${linerout_CFLAGS} ${es_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xflash_CFLAGS}

bin_PROGRAMS = densprof

densprof_SOURCES = src/dens_profiles.cpp
densprof_LDADD = ${es_LIBS} ${epsplot_LIBS} ${linerout_LIBS} ${es_LIBS} ${xmath_LIBS} ${xio_LIBS}  ${xstdlib_LIBS} ${xflash_LIBS}


