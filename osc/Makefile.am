AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${jsoncpp_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS} ${linerout_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS} ${xio_CFLAGS}

lib_LTLIBRARIES = libosc.la
libosc_la_SOURCES =  src/libOSC.cpp
libosc_la_LDFLAGS = -version-info 0:0:0


include_HEADERS = include/OSC.hpp

pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc


