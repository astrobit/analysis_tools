dnl ===========================================================================
dnl                http://autoconf-archive.cryp.to/acx_mpi.html
dnl ===========================================================================
dnl
dnl SYNOPSIS
dnl
dnl   ACX_DEF_LOCALLIB([local name],[additional CFLAGS], [additional LIBS],
dnl                       [directory relative to ${top_srcdir}], [lib name])
dnl
dnl DESCRIPTION
dnl
dnl   This macro defines <name>_CFLAGS and <name>_LIBS for libraries that
dnl   are generated within this project. This allows the libraries to be
dnl   treated similarly to external packages.
dnl
dnl   The optional 2nd and 3rd parameters allow the user to specify additional
dnl   cflags and libs, respectively, that are required for the package / library
dnl
dnl   The optional 4th argument specifies the directory in which the package / 
dnl   library is located if the directory is named differently than the name 
dnl   the user wishes to use for the flags.
dnl
dnl   The optional 5th argument specifies the actual library name if it differs
dnl   From the directory (if specified) or the local name
dnl
dnl LAST MODIFICATION
dnl
dnl   2017-10-25
dnl
dnl COPYLEFT
dnl
dnl   Copyright (c) 2017 Brian W. Mulligan <bwmulligan@astronaos.com>
dnl

AC_DEFUN([ACX_DEF_LOCALLIB],
[
AS_IF([test "x$4" = "x"],[
	AS_IF([test "x$5" = "x"],[
		AC_SUBST([$1_CFLAGS],['-I${top_srcdir}/$1/include $2'])
		AC_SUBST([$1_LIBS],['-L${top_srcdir}/$1 -l$1 $3'])
	],[
		AC_SUBST([$1_CFLAGS],['-I${top_srcdir}/$1/include $2'])
		AC_SUBST([$1_LIBS],['-L${top_srcdir}/$1 -l$5 $3'])
	])
],[
	AS_IF([test "x$5" = "x"],[
		AC_SUBST([$1_CFLAGS],['-I${top_srcdir}/$4/include $2'])
		AC_SUBST([$1_LIBS],['-L${top_srcdir}/$4 -l$4 $3'])
	],[
		AC_SUBST([$1_CFLAGS],['-I${top_srcdir}/$4/include $2'])
		AC_SUBST([$1_LIBS],['-L${top_srcdir}/$4 -l$5 $3'])
	])
])
])dnl ACX_DEF_LOCALLIB
