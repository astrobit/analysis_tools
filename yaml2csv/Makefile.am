AUTOMAKE_OPTIONS = foreign subdir-objects

bin_PROGRAMS = yaml2csv

AM_CPPFLAGS = -I${srcdir}/include ${yamlcpp_CFLAGS}

yaml2csv_SOURCES = src/yaml2csv.cpp
yaml2csv_LDADD =  ${yamlcpp_LIBS}

