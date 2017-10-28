AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = ${opacity_profile_CFLAGS} ${xflash_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bin_PROGRAMS = shex

shex_SOURCES = src/extract_shell.cpp
shex_LDADD =  ${xflash_LIBS} ${xio_LIBS} ${xstdlib_LIBS}


