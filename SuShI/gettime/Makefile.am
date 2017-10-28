AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xflash_CFLAGS}

bin_PROGRAMS = flashtime

flashtime_SOURCES = src/gettime.cpp
flashtime_LDADD =  ${xflash_LIBS}


