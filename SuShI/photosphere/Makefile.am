AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${opacity_profile_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS} ${xflash_CFLAGS}

bin_PROGRAMS = photosphere

photosphere_SOURCES = src/photosphere.cpp
photosphere_LDADD = ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS} ${xflash_LIBS}


