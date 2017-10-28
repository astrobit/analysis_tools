AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xastro_CFLAGS} ${xmath_CFLAGS}

bin_PROGRAMS = velfntest

velfntest_SOURCES = src/velfntest.cpp
velfntest_LDADD =  ${xastro_LIBS} ${xmath_LIBS} ${xstdlib_LIBS}

include_HEADERS = include/velocity_function.h

pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc
