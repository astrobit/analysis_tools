AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = plotgrid

plotgrid_SOURCES = src/plot_grid_quality.cpp
plotgrid_LDADD = ${epsplot_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


