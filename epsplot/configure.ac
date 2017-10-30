# Release version
m4_define([package_version_major],[1])
m4_define([package_version_minor],[0])
m4_define([package_version_micro],[0])
m4_define([package_repository],[https://github.com/astrobit/snatk])
m4_define([package_auth_email],[bwmulligan@astronaos.com])
 
# API version
m4_define([api_version],package_version_major)

AC_INIT(snatk-epsplot,package_version_major.package_version_minor.package_version_micro,package_auth_email,snatk-epsplot,package_repository)
#AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([PACKAGE_VERSION_MAJOR],package_version_major)
AC_SUBST([PACKAGE_VERSION_MINOR],package_version_minor)
AC_SUBST([PACKAGE_VERSION_MICRO],package_version_micro)
AC_SUBST([PACKAGE_REPOSITORY],package_repository)
AC_SUBST([PACKAGE_AUTH_EMAIL],package_auth_email)
AC_SUBST([API_VERSION],api_version)
AC_SUBST([LIB_NAME],[epsplot])
AC_SUBST([LIB_DESCRIPTION],["direct to eps plotting utility"])
AC_SUBST([LIB_REQUIRES],["xstdlib-1 >= 1.0.0"])


LT_INIT

AM_INIT_AUTOMAKE

AC_LANG(C++)

AC_PROG_CXX

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

AX_CXX_COMPILE_STDCXX(11,noext,mandatory) #use c++14 standard, required in a variety of places

AC_OPENMP #openmp required

PKG_CHECK_MODULES([xstdlib],[xstdlib-1],[],[AC_MSG_ERROR([" Could not find xstdlib development package (available at http://github.com/astrobit/xlibs)"])])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([$PACKAGE_NAME-$API_VERSION.pc:lib.pc.in],[],[PACKAGE_NAME='$PACKAGE_NAME' API_VERSION='$API_VERSION'])

AC_OUTPUT

