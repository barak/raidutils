#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([I2O RAID Utilities],[0.0.6],[],[i2o-raidutils],[http://i2o.shadowconnect.com])
AC_CONFIG_SRCDIR([raidutil/raidutil.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AR
LT_INIT

AX_CFLAGS_WARN_ALL
AX_CHECK_COMPILE_FLAG([-Wno-write-strings], [CFLAGS="$CFLAGS -Wno-write-strings"])
AX_CHECK_COMPILE_FLAG([-Wno-comments], [CFLAGS="$CFLAGS -Wno-comments"])
AC_LANG([C++])
AX_CXXFLAGS_WARN_ALL
AX_CHECK_COMPILE_FLAG([-Wno-write-strings], [CXXFLAGS="$CXXFLAGS -Wno-write-strings"])
AX_CHECK_COMPILE_FLAG([-Wno-comments], [CXXFLAGS="$CXXFLAGS -Wno-comments"])
AC_LANG([C])

# Checks for libraries.

# Checks for header files.
AC_LANG([C])
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h malloc.h memory.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/systeminfo.h unistd.h values.h inttypes.h])
AC_LANG([C++])
AC_CHECK_HEADERS([strstream.h sstream fstream.h fstream iostream.h iostream iomanip.h iomanip])
AC_LANG([C])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC

AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm atexit dup2 memset strchr strcspn strpbrk strrchr strspn strstr strtol strtoul sysinfo uname])

AM_PROG_CC_C_O

AC_CONFIG_FILES([Makefile
                 raideng/Makefile
                 raidutil/Makefile
                 lib/Makefile
                 distribution/raidutils.spec])
AC_OUTPUT
