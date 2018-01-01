AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${epsplot_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${linerout_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS}

lib_LTLIBRARIES = libesplot.la
libesplot_la_SOURCES =  src/Plot_Utilities.cpp
libesplot_la_LDFLAGS = -version-info 0:0:0


include_HEADERS = include/Plot_Utilities.h

# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

