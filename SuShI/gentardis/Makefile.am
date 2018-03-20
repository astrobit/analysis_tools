AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xastro_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${abundance_CFLAGS} ${opacity_profile_CFLAGS}

bin_PROGRAMS = gentardis

gentardis_SOURCES = src/gentardis.cpp
gentardis_LDADD = ${abundance_LIBS} ${xastro_LIBS} ${opacity_profile_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


