AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xflash_CFLAGS}

bin_PROGRAMS = 1dfm

1dfm_SOURCES = src/1dFlashMovie.cpp
1dfm_LDADD = ${epsplot_LIBS}  ${xio_LIBS}  ${xstdlib_LIBS}  ${xflash_LIBS}

