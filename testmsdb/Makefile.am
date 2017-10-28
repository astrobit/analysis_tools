AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${msdb_CFLAGS}  ${xstdlib_CFLAGS} ${es_CFLAGS} ${radiation_CFLAGS} ${veliocity_field_CFLAGS} ${opacity_profile_CFLAGS} ${linerout_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS}

bin_PROGRAMS = test_msdb
test_msdb_SOURCES = src/msdb_test.cpp

test_msdb_LDADD = ${es_LIBS} ${linerout_LIBS}  ${xmath_LIBS} ${xio_LIBS}  ${xstdlib_LIBS} ${msdb_LIBS}



