AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = spectra-subtract

spectra_subtract_SOURCES = src/spectra-subtract.cpp
spectra_subtract_LDADD = ${xio_LIBS} ${xstdlib_LIBS}

