AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${osc_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS}

bin_PROGRAMS = osc-extract-spectrum

osc_extract_spectrum_SOURCES = src/osc-extract.cpp
osc_extract_spectrum_LDADD = ${osc_LIBS}  ${xstdlib_LIBS} ${xastro_LIBS}

