AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include -DDATADIR=\"${pkgdatadir}\" ${xio_CFLAGS}

lib_LTLIBRARIES = libabundance.la
libabundance_la_SOURCES =  src/abundance_list.cpp
libabundance_la_LDFLAGS = -version-info 0:0:0



include_HEADERS = include/abundance.h
# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

pkgdata_DATA = data/CO_Rich.csv data/Solar.csv data/He_Solar.csv data/He_Solar_Ca_1p6.csv data/He_Solar_Ca_2p5.csv data/He_Solar_Ca_4p0.csv data/He_Solar_Ca_6p3.csv data/He_Solar_Ca_10.csv data/He_Solar_Ca_100.csv data/He_Solar_Ca_1000.csv data/He_Solar_Ca_10000.csv data/Shen_p005M.csv data/Shen_p01M.csv data/Shen_p02M.csv data/Solar_Ca_0p32.csv data/Solar_Ca_0p63.csv data/Solar_Ca_100.csv data/Solar_Ca_10.csv data/Solar_Ca_1p3.csv data/Solar_Ca_1p6.csv data/Solar_Ca_2p5.csv data/Solar_Ca_3p2.csv data/Solar_Ca_4p0.csv data/Solar_Ca_6p3.csv data/CO_Rich_Ca_0p63.csv data/CO_Rich_Ca_10000000.csv data/CO_Rich_Ca_1000000.csv data/CO_Rich_Ca_100000.csv data/CO_Rich_Ca_10000.csv data/CO_Rich_Ca_1000.csv data/CO_Rich_Ca_100.csv data/CO_Rich_Ca_10.csv data/CO_Rich_Ca_16.csv data/CO_Rich_Ca_1p6.csv data/CO_Rich_Ca_2p5.csv data/CO_Rich_Ca_4p0.csv data/CO_Rich_Ca_6p3.csv data/Seitenzahl_2013_N1.csv data/Seitenzahl_2013_N3.csv data/Seitenzahl_2013_N5.csv data/Seitenzahl_2013_N10.csv data/Seitenzahl_2013_N20.csv data/Seitenzahl_2013_N40.csv data/Seitenzahl_2013_N100.csv data/Seitenzahl_2013_N100L.csv data/Seitenzahl_2013_N100H.csv data/Seitenzahl_2013_N100_Z0p1.csv data/Seitenzahl_2013_N100_Z0p5.csv data/Seitenzahl_2013_N100_Z0p01.csv data/Seitenzahl_2013_N150.csv data/Seitenzahl_2013_N200.csv data/Seitenzahl_2013_N300C.csv  data/Seitenzahl_2013_N1600.csv  data/Seitenzahl_2013_N1600C.csv data/Seitenzahl_2013_RadioNuc_N1.csv data/Seitenzahl_2013_RadioNuc_N3.csv data/Seitenzahl_2013_RadioNuc_N5.csv data/Seitenzahl_2013_RadioNuc_N10.csv data/Seitenzahl_2013_RadioNuc_N20.csv data/Seitenzahl_2013_RadioNuc_N40.csv data/Seitenzahl_2013_RadioNuc_N100.csv data/Seitenzahl_2013_RadioNuc_N100L.csv data/Seitenzahl_2013_RadioNuc_N100H.csv data/Seitenzahl_2013_RadioNuc_N100_Z0p1.csv data/Seitenzahl_2013_RadioNuc_N100_Z0p5.csv data/Seitenzahl_2013_RadioNuc_N100_Z0p01.csv data/Seitenzahl_2013_RadioNuc_N150.csv data/Seitenzahl_2013_RadioNuc_N200.csv data/Seitenzahl_2013_RadioNuc_N300C.csv  data/Seitenzahl_2013_RadioNuc_N1600.csv  data/Seitenzahl_2013_RadioNuc_N1600C.csv
