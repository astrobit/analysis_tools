# Release version
m4_define([package_version_major],[2])
m4_define([package_version_minor],[0])
m4_define([package_version_micro],[0])
m4_define([package_repository],[https://github.com/astrobit/snatk])
m4_define([package_auth_email],[bwmulligan@astronaos.com])
 
# API version
m4_define([api_version],package_version_major)

AC_INIT(snatk-linerout,package_version_major.package_version_minor.package_version_micro,package_auth_email,snatk-linerout,package_repository)
AC_CONFIG_MACRO_DIR([../m4])

AC_SUBST([PACKAGE_VERSION_MAJOR],package_version_major)
AC_SUBST([PACKAGE_VERSION_MINOR],package_version_minor)
AC_SUBST([PACKAGE_VERSION_MICRO],package_version_micro)
AC_SUBST([PACKAGE_REPOSITORY],package_repository)
AC_SUBST([PACKAGE_AUTH_EMAIL],package_auth_email)
AC_SUBST([API_VERSION],api_version)
AC_SUBST([LIB_NAME],[linerout])
AC_SUBST([LIB_DESCRIPTION],["routines relating to atomic line spectra for use with es and SuShI"])
AC_SUBST([LIB_REQUIRES],["xio-1 >= 1.0.0 xmath-1 >= 1.0.0 es-0 >= 0.100.0 snatk-radiation-1 >= 1.0.0 snatk-msdb-1 >= 1.0.0 snatk-SuShI-opacity_profile-1 >= 1.0.0"])


LT_INIT

AM_INIT_AUTOMAKE

AC_LANG(C++)

AC_PROG_CXX

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

AX_CXX_COMPILE_STDCXX(11,noext,mandatory) #use c++14 standard, required in a variety of places

AC_OPENMP #openmp required

PKG_CHECK_MODULES([es],[es-0],[],[AC_MSG_ERROR([" Could not find libes development package (available at http://github.com/astrobit/es)"])])
PKG_CHECK_MODULES([xio],[xio-1],[],[AC_MSG_ERROR([" Could not find xio development package (available at http://github.com/astrobit/xlibs)"])])
PKG_CHECK_MODULES([xmath],[xmath-1],[],[AC_MSG_ERROR([" Could not find xmath development package (available at http://github.com/astrobit/xlibs)"])])

AC_SUBST([radiation_CFLAGS],['-I${top_srcdir}/../radiation/include ${xastro_CFLAGS}'])
AC_SUBST([radiation_LIBS],['${xastro_LIBS}'])
AC_SUBST([velocity_field_CFLAGS],['-I${top_srcdir}/../velocityfield/include ${xastro_CFLAGS}'])
AC_SUBST([velocity_field_LIBS],['${xastro_LIBS}'])
AC_SUBST([opacity_profile_CFLAGS],['-I${top_srcdir}/../SuShI/opacity_profile/include'])
AC_SUBST([opacity_profile_LIBS],[])
AC_SUBST([msdb_CFLAGS],['-I${top_srcdir}/../msdb/include ${es_CFLAGS}'])
AC_SUBST([msdb_LIBS],['-L${top_srcdir}/../msdb ${es_LIBS}'])


#ACX_DEF_LOCALLIB([statepop],[],[],[],[sp])
#ACX_DEF_LOCALLIB([msdb],[${es_CFLAGS}],[${es_LIBS}])
#ACX_DEF_LOCALLIB([epsplot],[],[${xstdlib_LIBS}])
#ACX_DEF_LOCALLIB([osc],[${jsoncpp_CFLAGS}],[${jsoncpp_LIBS}])
#ACX_DEF_LOCALLIB([opacity_project],[${xastro_CFLAGS} ${radiation_CFLAGS} ${velocity_field_CFLAGS}],[${xastro_LIBS} ${radiation_LIBS} ${velocity_field_LIBS}])
#ACX_DEF_LOCALLIB([linerout],[${xio_CFLAGS} ${xmath_CFLAGS} ${es_CFLAGS} ${radiation_CFLAGS} ${msdb_CFLAGS} ${opacity_profile_CFLAGS}],[${xastro_LIBS} ${xio_LIBS} ${xmath_LIBS} ${es_LIBS} ${radiation_LIBS} ${msdb_LIBS} ${opacity_profile_LIBS}])
#ACX_DEF_LOCALLIB([esplot],[${es_CFLAGS} ${eps_CFLAGS} ${xio_CFLAGS}],[${es_LIBS} ${eps_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${linerout_LIBS}])
#ACX_DEF_LOCALLIB([kurucz_data],[${xastro_CFLAGS} ${xmath_CFLAGS} ${linerout_CFLAGS} ${radiation_CFLAGS}],[${xastro_LIBS} ${xmath_LIBS} ${linerout_LIBS} ${radiation_LIBS}])


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([$PACKAGE_NAME-$API_VERSION.pc:lib.pc.in],[],[PACKAGE_NAME='$PACKAGE_NAME' API_VERSION='$API_VERSION'])

AC_OUTPUT

