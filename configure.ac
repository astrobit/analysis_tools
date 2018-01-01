# Release version
m4_define([package_version_major],[3])
m4_define([package_version_minor],[2])
m4_define([package_version_micro],[0])
m4_define([package_repository],[https://github.com/astrobit/snatk])
m4_define([package_auth_email],[bwmulligan@astronaos.com])
 
# API version
m4_define([api_version],package_version_major)

AC_INIT(snatk,package_version_major.package_version_minor.package_version_micro,package_auth_email,snatk,package_repository)
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([PACKAGE_VERSION_MAJOR],package_version_major)
AC_SUBST([PACKAGE_VERSION_MINOR],package_version_minor)
AC_SUBST([PACKAGE_VERSION_MICRO],package_version_micro)
AC_SUBST([PACKAGE_REPOSITORY],package_repository)
AC_SUBST([PACKAGE_AUTH_EMAIL],package_auth_email)
AC_SUBST([API_VERSION],api_version)


LT_INIT

AM_INIT_AUTOMAKE([foreign])

AC_LANG(C++)

AC_PROG_CXX

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

MOD_PROG_ENVMOD
MOD_INSTALLDIR

AX_CXX_COMPILE_STDCXX(11,noext,mandatory) #use c++14 standard, required in a variety of places

AC_OPENMP #openmp required

AX_PTHREAD #needed for anything using BWM801 engine

AC_CHECK_HEADERS([hdf5.h],[AC_SUBST([hdf5_LIBS],[-lhdf5])],[AC_MSG_ERROR([" Could not find hdf5 development package"])]) # make sure HDF5 is installed - 


PKG_CHECK_MODULES([xstdlib],[xstdlib-1],[],[AC_MSG_ERROR([" Could not find xstdlib development package"])])
PKG_CHECK_MODULES([xastro],[xastro-1 >= 1.2.0],[],[AC_MSG_ERROR([" Could not find xastro v1.2.0 or later development package"])])
PKG_CHECK_MODULES([xmath],[xmath-1],[],[AC_MSG_ERROR([" Could not find xmath development package"])])
PKG_CHECK_MODULES([xio],[xio-1],[],[AC_MSG_ERROR([" Could not find xio development package"])])
PKG_CHECK_MODULES([xflash],[xflash-1],[],[AC_MSG_ERROR([" Could not find xflash development package"])])
PKG_CHECK_MODULES([es],[es-0],[],[AC_MSG_ERROR([" Could not find libes development package"])])
PKG_CHECK_MODULES([jsoncpp],[jsoncpp],[],[AC_MSG_ERROR([" Could not find jsoncpp development package"])])
PKG_CHECK_MODULES([xml2],[libxml-2.0],[],[AC_MSG_ERROR([" Could not find xml2 development package"])])
PKG_CHECK_MODULES([yamlcpp],[yaml-cpp],[],[AC_MSG_ERROR([" Could not find yaml-cpp development package"])])
PKG_CHECK_MODULES([bwm801],[BWM801-0 >= 0.4.0],[AM_CONDITIONAL([BWM801AVAIL],[test "1" = "1"])],[AM_CONDITIONAL([BWM801AVAIL],[test "1" = "2"])])


AC_SUBST([radiation_CFLAGS],['-I${top_srcdir}/radiation/include ${xastro_CFLAGS}'])
AC_SUBST([radiation_LIBS],['${xastro_LIBS}'])
AC_SUBST([velocity_field_CFLAGS],['-I${top_srcdir}/velocityfield/include ${xastro_CFLAGS}'])
AC_SUBST([velocity_field_LIBS],['${xastro_LIBS}'])
AC_SUBST([opacity_profile_CFLAGS],['-I${top_srcdir}/SuShI/opacity_profile/include  ${xio_CFLAGS}'])
AC_SUBST([opacity_profile_LIBS],['-L${top_srcdir}/SuShI/opacity_profile -lopacity_profile ${xio_LIBS} ${xstdlib_LIBS}'])


ACX_DEF_LOCALLIB([statepop],[],[],[],[sp])
ACX_DEF_LOCALLIB([msdb],[${es_CFLAGS}],[${es_LIBS}])
ACX_DEF_LOCALLIB([epsplot],[],[${xstdlib_LIBS}])
ACX_DEF_LOCALLIB([osc],[${jsoncpp_CFLAGS}],[${jsoncpp_LIBS}])
ACX_DEF_LOCALLIB([opacity_project],[${xastro_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS}],[${xastro_LIBS} ${radiation_LIBS} ${velocity_field_LIBS}])
ACX_DEF_LOCALLIB([linerout],[${xio_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS}],[${xastro_LIBS} ${xio_LIBS} ${xmath_LIBS} ${es_LIBS} ${radiation_LIBS}  ${velocity_field_LIBS} ${msdb_LIBS} ${opacity_profile_LIBS}])
ACX_DEF_LOCALLIB([esplot],[${es_CFLAGS} ${eps_CFLAGS} ${xio_CFLAGS}],[${es_LIBS} ${eps_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${linerout_LIBS}])
ACX_DEF_LOCALLIB([kurucz_data],[${xastro_CFLAGS} ${xmath_CFLAGS} ${linerout_CFLAGS} ${radiation_CFLAGS}],[${xastro_LIBS} ${xmath_LIBS} ${linerout_LIBS} ${radiation_LIBS}])

AC_CONFIG_SUBDIRS([msdb])
AC_CONFIG_SUBDIRS([radiation])
AC_CONFIG_SUBDIRS([velocityfield])
AC_CONFIG_SUBDIRS([statepop])
AC_CONFIG_SUBDIRS([epsplot])
AC_CONFIG_SUBDIRS([osc])
AC_CONFIG_SUBDIRS([opacity_project])
AC_CONFIG_SUBDIRS([linerout])
AC_CONFIG_SUBDIRS([esplot])
AC_CONFIG_SUBDIRS([kurucz_data])
AC_CONFIG_SUBDIRS([SuShI/opacity_profile])
AC_CONFIG_SUBDIRS([SuShI/spectra-fit])


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([testmsdb/Makefile])
AC_CONFIG_FILES([1dFlashMovie/Makefile])
AC_CONFIG_FILES([compositions/Makefile])
AC_CONFIG_FILES([equivwidth/Makefile])
AC_CONFIG_FILES([es_gaussian/Makefile])
AC_CONFIG_FILES([genplot/Makefile])
AC_CONFIG_FILES([linfit/Makefile])
AC_CONFIG_FILES([max/Makefile])
AC_CONFIG_FILES([min/Makefile])
AC_CONFIG_FILES([plotspectrum/Makefile])
AC_CONFIG_FILES([Vega_filters/Makefile])
AC_CONFIG_FILES([yaml2csv/Makefile])
AC_CONFIG_FILES([SuShI/Makefile])
AC_CONFIG_FILES([SuShI/densprof/Makefile])
AC_CONFIG_FILES([SuShI/FitViz/Makefile])
AC_CONFIG_FILES([SuShI/flash2snec/Makefile])
AC_CONFIG_FILES([SuShI/gather_photometry/Makefile])
AC_CONFIG_FILES([SuShI/genjsonfit/Makefile])
AC_CONFIG_FILES([SuShI/gentardis/Makefile])
AC_CONFIG_FILES([SuShI/getopdata/Makefile])
AC_CONFIG_FILES([SuShI/gettime/Makefile])
AC_CONFIG_FILES([SuShI/multiion/Makefile])
AC_CONFIG_FILES([SuShI/mve/Makefile])
AC_CONFIG_FILES([SuShI/mve_vels/Makefile])
AC_CONFIG_FILES([SuShI/obsolete/Makefile])
AC_CONFIG_FILES([SuShI/opmaptest/Makefile])
AC_CONFIG_FILES([SuShI/OSCIn-SuShI/Makefile])
AC_CONFIG_FILES([SuShI/photosphere/Makefile])
AC_CONFIG_FILES([SuShI/plotgridqual/Makefile])
AC_CONFIG_FILES([SuShI/quikplotFlash/Makefile])
AC_CONFIG_FILES([SuShI/shex/Makefile])


AS_IF( [test ${MODTYPE} = "LMOD"],
	[AC_CONFIG_FILES([$PACKAGE_VERSION.lua:modfile.lua.in])],
	[AS_IF( [test ${MODTYPE} = "ENVMOD"], [AC_CONFIG_FILES([$PACKAGE_VERSION:modfile.in])], [])])



AC_OUTPUT

