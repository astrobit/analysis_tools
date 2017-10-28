AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${epsplot_CFLAGS} ${linerout_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS} ${xstdlib_CFLAGS} ${jsoncpp_CFLAGS}

bin_PROGRAMS = genjsonfit

genjsonfit_SOURCES = src/gen_json_fit_list.cpp
genjsonfit_LDADD =   ${epsplot_LIBS} ${linerout_LIBS} ${xastro_LIBS} ${xmath_LIBS} ${xstdlib_LIBS} ${jsoncpp_LIBS}

