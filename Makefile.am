AUTOMAKE_OPTIONS = foreign subdir-objects

SUBDIRS =  abundance epsplot radiation velocityfield msdb linerout esplot plot-tardis-abundance kurucz_data opacity_project osc osc-extract SuShI 1dFlashMovie compositions equivwidth es_gaussian genplot linfit max min plotspectrum  statepop Vega_filters yaml2csv  

if INSTALLMOD
if HAVEENVMOD
modfile_DATA = ${PACKAGE_VERSION}
else
modfile_DATA = ${PACKAGE_VERSION}.lua
endif
endif
