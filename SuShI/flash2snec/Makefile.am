AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xflash_CFLAGS}

bin_PROGRAMS = flash2snec

flash2snec_SOURCES = src/FLASH_2_SNEC.cpp
flash2snec_LDADD = ${xio_LIBS} ${xstdlib_LIBS} ${xflash_LIBS}


