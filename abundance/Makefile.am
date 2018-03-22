AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include -DDATADIR=\"${pkgdatadir}\" ${xio_CFLAGS}

lib_LTLIBRARIES = libabundance.la
libabundance_la_SOURCES =  src/abundance_list.cpp
libabundance_la_LDFLAGS = -version-info 0:0:0



include_HEADERS = include/abundance.h
# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

pkgdata_DATA = data/CO_Rich.csv data/Seitenzahl_N100_2013_Ca_Rich.csv data/Seitenzahl_N100_2013.csv data/Solar.csv data/He_Solar.csv data/He_Solar_Ca_1p6.csv data/He_Solar_Ca_2p5.csv data/He_Solar_Ca_4p0.csv data/He_Solar_Ca_6p3.csv data/He_Solar_Ca_10.csv



