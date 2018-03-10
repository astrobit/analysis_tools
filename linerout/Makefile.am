AUTOMAKE_OPTIONS = foreign subdir-objects
AM_CPPFLAGS = -I${srcdir}/include ${es_CFLAGS} ${epsplot_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS} ${xmath_CFLAGS}

lib_LTLIBRARIES = liblinerout.la
liblinerout_la_SOURCES =  src/line_routines_ccm_dered.cpp src/line_routines.cpp src/line_routines_equivalent_width.cpp src/line_routines_find_flux.cpp src/line_routines_find_minimum.cpp src/line_routines_gaussian_fit.cpp src/line_routines_get_norm_const.cpp src/line_routines_grad_des_refine_fit.cpp src/line_routines_grid_refine_fit.cpp src/line_routines_libes.cpp src/line_routines_msdb_load_generate.cpp src/line_routines_normalization.cpp src/line_routines_radiation.cpp src/line_routines_refine_prep.cpp src/line_routines_spectral_database.cpp src/line_routines_stats.cpp src/line_routines_volume.cpp src/line_routines_photoionization_rate.cpp
liblinerout_la_LDFLAGS = -version-info 2:0:0



include_HEADERS = include/line_routines.h
# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = ${PACKAGE_NAME}-${API_VERSION}.pc

noinst_PROGRAMS = ion_cs_test
ion_cs_test_SOURCES = src/ion_cs_test.cpp
ion_cs_test_LDADD = ${xastro_LIBS} ${velocity_field_LIBS} ${radiation_LIBS}
ion_cs_test_CFLAGS = ${AM_CPPFLAGS} ${XASTRO_CFLAGS}

