AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xio_CFLAGS} ${xstdlib_CFLAGS}

lib_LTLIBRARIES = libepsplot.la
libepsplot_la_SOURCES =  src/ep_axis.cpp src/ep_color.cpp src/ep_epsfile.cpp src/ep_errorbar.cpp src/ep_lineplot.cpp src/ep_plot3d.cpp src/ep_rectangle.cpp src/eps_plot.cpp src/ep_stipple.cpp src/ep_symbol.cpp src/ep_text.cpp
libepsplot_la_LDFLAGS = -version-info 0:0:0


bin_PROGRAMS = testeps
testeps_SOURCES = src/testeps.cpp
testeps_LDADD = -lepsplot ${xio_LIBS} ${xstdlib_LIBS}

include_HEADERS = include/eps_plot.h

# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

