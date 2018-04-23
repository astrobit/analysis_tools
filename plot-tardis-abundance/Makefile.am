AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = ${epsplot_CFLAGS}  ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS}

bin_PROGRAMS = plot-tardis-abundance

plot_tardis_abundance_SOURCES = src/plot-tardis-abundance.cpp
plot_tardis_abundance_LDADD = ${epsplot_LIBS}  ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS}

