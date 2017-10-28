AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = mve_vels

mve_vels_SOURCES = src/mve_velocity_plot.cpp
mve_vels_LDADD =  ${xio_LIBS} ${xstdlib_LIBS}

