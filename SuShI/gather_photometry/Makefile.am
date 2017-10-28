AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = gather_photometry

gather_photometry_SOURCES = src/gather_photometry.cpp
gather_photometry_LDADD = ${xio_LIBS} ${xstdlib_LIBS}


