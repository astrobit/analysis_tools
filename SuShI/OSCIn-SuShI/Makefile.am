AUTOMAKE_OPTIONS = foreign subdir-objects

if BWM801AVAIL
AM_CPPFLAGS = -I${srcdir}/include ${opacity_profile_CFLAGS} ${epsplot_CFLAGS} ${linerout_CFLAGS} ${msdb_CFLAGS}  ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS} ${bwm801_CFLAGS} ${jsoncpp_CFLAGS} ${osc_CFLAGS}

bin_PROGRAMS = OSCIn-SuShI

OSCIn_SuShI_SOURCES = src/OSCIn-SuShI.cpp src/OS_gfx.cpp src/OS_misc.cpp src/OS_UI.cpp
OSCIn_SuShI_LDADD = ${osc_LIBS} ${jsoncpp_LIBS} ${linerout_LIBS} ${epsplot_LIBS} ${msdb_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xmath_LIBS} ${es_LIBS} ${bwm801_LIBS}
endif

