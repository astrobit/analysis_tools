AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${xio_CFLAGS}  ${xstdlib_CFLAGS}  ${xml2_CFLAGS} -DDATAPATH=\"${pkgdatadir}\"

bin_PROGRAMS = genplot

genplot_SOURCES = src/genplot.cpp
genplot_LDADD = ${epsplot_LIBS}  ${xio_LIBS}  ${xstdlib_LIBS} ${xml2_LIBS}

pkgdata_DATA = data/genplot_example2.xml data/genplot_example3.xml data/genplot_example.csv data/genplot_example.xml data/genplots.dtd
