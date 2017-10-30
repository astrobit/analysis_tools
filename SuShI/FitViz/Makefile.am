AUTOMAKE_OPTIONS = foreign subdir-objects

if BWM801AVAIL
bin_PROGRAMS = FitViz
AM_CPPFLAGS = -I${srcdir}/./include ${linerout_CFLAGS} ${epsplot_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS} ${bwm801_CFLAGS}

FitViz_SOURCES = src/FitViz.cpp src/Fit_Viz_gfx.cpp src/Fit_Viz_manfit.cpp src/Fit_Viz_misc.cpp src/Fit_Viz_UI.cpp
FitViz_LDADD = ${linerout_LIBS} ${epsplot_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xmath_LIBS} ${es_LIBS} ${bwm801_LIBS}
endif

