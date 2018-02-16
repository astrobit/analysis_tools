AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xastro_CFLAGS}

include_HEADERS = include/radiation.h

pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

noinst_PROGRAMS = testrad
testrad_SOURCES = src/testrad.cpp
testrad_LDFLAGS = ${xastro_LIBS}
testrad_CPPFLAGS = ${xastro_CFLAGS} -I./include

