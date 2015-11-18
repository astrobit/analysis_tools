all: Plot_Utilities lineanal2 msdb photosphere reverse rlamc yaml2csv shex densprof quikplot quikplotspec flashtime spectrafit userprof userseries gaussianprof quikplotseries equivwidth line_routines bestfitcsv combinedensdata ionabddet paperplot seriesewvmin gatherfits genfitmom modfits psfit psfitinter genfs min max data2databin ungatherfits tempex velev regenfits fixfits replot modelvelev diffusion flash2snec mve_vels multiion testeps libcomp sahatest genplot gentardis gausstest gather_pEW_vels test_msdb Vega_filters gather_photometry gather_scalars

.PHONY: all

CCOMP=mpicxx
INCLUDEDIR=./include
SRCDIR=./src
BINDIR=./bin
TMPDIR=./obj
LIBDIR=./lib
CFLAGS=-DMPICH_IGNORE_CXX_SEEK=1 -c -I$(INCLUDEDIR)
CLFLAGS=-I$(INCLUDEDIR) -L$(LIBDIR)
LFLAGS=-DMPICH_IGNORE_CXX_SEEK=1 -I$(INCLUDEDIR) -L$(LIBDIR)
LIBCOMP=ar
LIBCOMPFLAG=-cvr
PLOTUTILLIB=-lplotutil
ESFLAGS= -fopenmp
ESLIBS= -les -lm -lyaml-cpp -lcfitsio 
XLIBSPATH=~/xlibs
XLIBSCHANGE=$(XLIBSPATH)/lib/libxio.a $(XLIBSPATH)/lib/libxstdlib.a $(XLIBSPATH)/lib/libxmath.a $(XLIBSPATH)/lib/libxastro.a $(XLIBSPATH)/lib/libxflash.a
XMLINCLUDE= -I/usr/include/libxml2

$(LIBDIR)/libplotutil.a: $(SRCDIR)/Plot_Utilities.cpp $(INCLUDEDIR)/Plot_Utilities.h $(XLIBSCHANGE) $(INCLUDEDIR)/eps_plot.h $(SRCDIR)/eps_plot.cpp
	$(CCOMP) $(CFLAGS) $(SRCDIR)/Plot_Utilities.cpp -o $(TMPDIR)/Plot_Utilities.o
	$(CCOMP) $(CFLAGS) $(SRCDIR)/eps_plot.cpp -o $(TMPDIR)/eps_plot.o
	-rm $(LIBDIR)/libplotutil.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libplotutil.a $(TMPDIR)/eps_plot.o $(TMPDIR)/Plot_Utilities.o 
Plot_Utilities: $(LIBDIR)/libplotutil.a

$(LIBDIR)/liblinerout.a: $(SRCDIR)/line_routines.cpp $(INCLUDEDIR)/line_routines.h $(XLIBSCHANGE)
	$(CCOMP) $(CFLAGS) $(SRCDIR)/line_routines.cpp -o $(TMPDIR)/line_routines.o
	-rm $(LIBDIR)/liblinerout.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/liblinerout.a $(TMPDIR)/line_routines.o
line_routines: $(LIBDIR)/liblinerout.a

msdb: $(LIBDIR)/libmsdb.a
$(LIBDIR)/libmsdb.a: $(SRCDIR)/model_spectra_db.cpp $(INCLUDEDIR)/model_spectra_db.h
	$(CCOMP) $(CFLAGS) $(ESFLAGS) $(SRCDIR)/model_spectra_db.cpp -o $(TMPDIR)/model_spectra_db.o
	-rm $(LIBDIR)/libmsdb.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libmsdb.a $(TMPDIR)/model_spectra_db.o

$(OBJDIR)/line_anal.o: $(SRCDIR)/line_anal.cpp
	$(CCOMP) $(CFLAGS) $(SRCDIR)/line_anal.cpp  -o $(OBJDIR)/line_anal.o

$(OBJDIR)/line_analysis.o: $(SRCDIR)/line_analysis.cpp
	$(CCOMP) $(CFLAGS) $(SRCDIR)/line_analysis.cpp  -o $(OBJDIR)/line_analysis.o

$(BINDIR)/line_anal: $(OBJDIR)/line_anal.o $(OBJDIR)/line_analysis.o $(XLIBSCHANGE)
	$(CCOMP) $(LFLAGS) $(OBJDIR)/line_analysis.o $(OBJDIR)/line_anal.o  -lxmath -lxio -lxstdlib -lxastro -o $(BINDIR)/line_anal
line_anal: $(BINDIR)/line_anal

$(BINDIR)/lineanal2: $(SRCDIR)/line_analysis2.cpp $(XLIBSCHANGE)
	$(CCOMP) $(LFLAGS) $(SRCDIR)/line_analysis2.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -lhdf5 -lxflash -o $(BINDIR)/lineanal2
lineanal2: $(BINDIR)/lineanal2

$(BINDIR)/lineanal4: $(SRCDIR)/line_analysis4.cpp $(XLIBSCHANGE)
	$(CCOMP) $(LFLAGS) $(SRCDIR)/line_analysis4.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/lineanal4
lineanal4: $(BINDIR)/lineanal4

$(BINDIR)/lineanal5: $(SRCDIR)/line_analysis5.cpp $(XLIBSCHANGE)
	$(CCOMP) $(LFLAGS) $(SRCDIR)/line_analysis5.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/lineanal5
lineanal5: $(BINDIR)/lineanal5

$(BINDIR)/photosphere: $(SRCDIR)/photosphere.cpp $(XLIBSCHANGE)
	$(CCOMP) $(LFLAGS) $(SRCDIR)/photosphere.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -lhdf5 -lxflash -o $(BINDIR)/photosphere
photosphere: $(BINDIR)/photosphere

$(BINDIR)/reverse_line_anal: $(SRCDIR)/reverse_line_anal.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/reverse_line_anal.cpp -lyaml-cpp -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/reverse_line_anal
reverse: $(BINDIR)/reverse_line_anal

$(BINDIR)/rlamc: $(SRCDIR)/rla_mass_combiner.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/rla_mass_combiner.cpp -lxio -lxstdlib -o $(BINDIR)/rlamc
rlamc: $(BINDIR)/rlamc

$(BINDIR)/yaml2csv: $(SRCDIR)/yaml2csv.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/yaml2csv.cpp -lyaml-cpp -o $(BINDIR)/yaml2csv
yaml2csv: $(BINDIR)/yaml2csv

$(BINDIR)/shex: $(SRCDIR)/extract_shell.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/extract_shell.cpp -lhdf5 -lxflash -lxio -lxstdlib -o $(BINDIR)/shex
shex: $(BINDIR)/shex

$(BINDIR)/densprof: $(SRCDIR)/dens_profiles.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/dens_profiles.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/densprof
densprof: $(BINDIR)/densprof

$(BINDIR)/quikplot: $(SRCDIR)/quikplot.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/quikplot.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/quikplot
quikplot: $(BINDIR)/quikplot

$(BINDIR)/quikplotspec: $(SRCDIR)/quikplotspectrum.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/quikplotspectrum.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/quikplotspec
quikplotspec: $(BINDIR)/quikplotspec

$(BINDIR)/flashtime: $(SRCDIR)/gettime.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gettime.cpp -lhdf5 -lxflash -o $(BINDIR)/flashtime
flashtime: $(BINDIR)/flashtime

$(BINDIR)/spectrafit: $(SRCDIR)/specta_fit.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/specta_fit.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/spectrafit
spectrafit: $(BINDIR)/spectrafit


$(BINDIR)/userprof: $(SRCDIR)/user_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/user_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/userprof
userprof: $(BINDIR)/userprof

$(BINDIR)/userseries: $(LIBDIR)/libplotutil.a $(SRCDIR)/user_series.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/user_series.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/userseries
userseries: $(BINDIR)/userseries

$(BINDIR)/gaussianprof: $(SRCDIR)/gaussian_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gaussian_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/gaussianprof
gaussianprof: $(BINDIR)/gaussianprof

quikplotseries: $(BINDIR)/quikplotseries 
$(BINDIR)/quikplotseries: $(SRCDIR)/quikplotseries.cpp $(LIBDIR)/libplotutil.a  $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/quikplotseries.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/quikplotseries

equivwidth: $(BINDIR)/equivwidth
$(BINDIR)/equivwidth: $(SRCDIR)/equivalentwidth.cpp $(LIBDIR)/liblinerout.a $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/equivalentwidth.cpp $(ESFLAGS) -llinerout -lxio -lxstdlib -lxmath $(ESLIBS) -o $(BINDIR)/equivwidth

bestfitcsv: $(BINDIR)/bestfitcsv
$(BINDIR)/bestfitcsv: $(SRCDIR)/bestfittocsv.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/bestfittocsv.cpp $(ESFLAGS) -lxio -lxstdlib $(ESLIBS) -o $(BINDIR)/bestfitcsv

combinedensdata: $(BINDIR)/combinedensdata
$(BINDIR)/combinedensdata: $(SRCDIR)/combinedensdata.cpp
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/combinedensdata.cpp -o $(BINDIR)/combinedensdata

ionabddet: $(BINDIR)/ionabddet
$(BINDIR)/ionabddet: $(SRCDIR)/ion_abd_determination.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/ion_abd_determination.cpp -lhdf5 -lxflash -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/ionabddet

paperplot: $(BINDIR)/paperplot
$(BINDIR)/paperplot: $(LIBDIR)/libplotutil.a $(SRCDIR)/paper_plots.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/paper_plots.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/paperplot

seriesewvmin: $(BINDIR)/seriesewvmin
$(BINDIR)/seriesewvmin: $(LIBDIR)/libplotutil.a $(SRCDIR)/paper_plots.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/ES_Vel_min.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/seriesewvmin

gatherfits: $(BINDIR)/gatherfits
$(BINDIR)/gatherfits: $(SRCDIR)/gather_best_fit_data.cpp $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gather_best_fit_data.cpp $(ESFLAGS)  $(ESLIBS) -lxstdlib -o $(BINDIR)/gatherfits

genfitmom: $(BINDIR)/genfitmom
$(BINDIR)/genfitmom: $(SRCDIR)/generate_fit_moments.cpp $(LIBDIR)/liblinerout.a $(XLIBSCHANGE) $(SRCDIR)/eps_plot.cpp
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/generate_fit_moments.cpp $(SRCDIR)/eps_plot.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/genfitmom

modfits: $(BINDIR)/modfits
$(BINDIR)/modfits: $(SRCDIR)/mod_best_fit_file.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/mod_best_fit_file.cpp $(ESFLAGS) $(ESLIBS) -lxstdlib -o $(BINDIR)/modfits

$(BINDIR)/psfit: $(SRCDIR)/PS_temp.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/PS_temp.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/psfit
psfit: $(BINDIR)/psfit

$(BINDIR)/psfitinter: $(SRCDIR)/PS_temp_inter.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/PS_temp_inter.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/psfitinter
psfitinter: $(BINDIR)/psfitinter

genfs: $(BINDIR)/genfs
$(BINDIR)/genfs: $(SRCDIR)/generate_flattened_spectra.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/generate_flattened_spectra.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/genfs

min: $(BINDIR)/min
$(BINDIR)/min: $(SRCDIR)/min.cpp
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/min.cpp -o $(BINDIR)/min

max: $(BINDIR)/max
$(BINDIR)/max: $(SRCDIR)/max.cpp
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/max.cpp -o $(BINDIR)/max

data2databin: $(BINDIR)/data2databin
$(BINDIR)/data2databin: $(SRCDIR)/data2databin.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/data2databin.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/data2databin

#data2csv: $(BINDIR)/data2csv
#$(BINDIR)/data2csv: $(SRCDIR)/data2csv.cpp $(INCLUDEDIR)/best_fit_data.h
#	$(CCOMP) $(CLFLAGS) $(SRCDIR)/data2csv.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/data2csv

#csv2data: $(BINDIR)/csv2data
#$(BINDIR)/csv2data: $(SRCDIR)/csv2data.cpp $(INCLUDEDIR)/best_fit_data.h
#	$(CCOMP) $(CLFLAGS) $(SRCDIR)/csv2data.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/csv2data

ungatherfits: $(BINDIR)/ungatherfits
$(BINDIR)/ungatherfits: $(SRCDIR)/ungather_best_fit_data.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/ungather_best_fit_data.cpp $(ESFLAGS)  $(ESLIBS) -lxstdlib -o $(BINDIR)/ungatherfits

tempex: $(BINDIR)/tempex
$(BINDIR)/tempex: $(SRCDIR)/temp_extractor.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/temp_extractor.cpp  -lhdf5 -lxflash -o $(BINDIR)/tempex

$(BINDIR)/velev: $(SRCDIR)/nofit_user_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/nofit_user_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/velev
velev: $(BINDIR)/velev


$(BINDIR)/regenfits: $(SRCDIR)/regen_data_file.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/regen_data_file.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/regenfits
regenfits: $(BINDIR)/regenfits

$(BINDIR)/fixfits: $(SRCDIR)/fix_data_file.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/fix_data_file.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/fixfits
fixfits: $(BINDIR)/fixfits

$(BINDIR)/replot: $(SRCDIR)/replot.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/replot.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/replot
replot: $(BINDIR)/replot

modelvelev:$(BINDIR)/modelvelev
$(BINDIR)/modelvelev: $(SRCDIR)/model_vel_evolution.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h $(LIBDIR)/libmsdb.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/model_vel_evolution.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxastro -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/modelvelev

diffusion: $(BINDIR)/diffusion
$(BINDIR)/diffusion: $(SRCDIR)/diffusion.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/diffusion.cpp -lxmath -lxio -lxstdlib -o $(BINDIR)/diffusion

flash2snec: $(BINDIR)/flash2snec
$(BINDIR)/flash2snec: $(SRCDIR)/FLASH_2_SNEC.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/FLASH_2_SNEC.cpp -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/flash2snec

mve_vels: $(BINDIR)/mve_vels
$(BINDIR)/mve_vels: $(SRCDIR)/mve_velocity_plot.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/mve_velocity_plot.cpp -lxio -lxstdlib -o $(BINDIR)/mve_vels

multiion: $(BINDIR)/multiion
$(BINDIR)/multiion: $(SRCDIR)/multiion_spectra.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libcomp.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/multiion_spectra.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -lcomp -llinerout -lxmath -lxastro -lxio -lxstdlib -o $(BINDIR)/multiion

testeps: $(BINDIR)/testeps
$(BINDIR)/testeps: $(SRCDIR)/testeps.cpp  $(LIBDIR)/libplotutil.a $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/testeps.cpp $(ESFLAGS) $(PLOTUTILLIB) -lxio -lxstdlib -o $(BINDIR)/testeps

libcomp: $(LIBDIR)/libcomp.a
$(LIBDIR)/libcomp.a: $(SRCDIR)/compositions.cpp $(INCLUDEDIR)/compositions.h  $(XLIBSCHANGE)
	$(CCOMP) $(CFLAGS) $(ESFLAGS) $(SRCDIR)/compositions.cpp -o $(TMPDIR)/compositions.o
	-rm $(LIBDIR)/libcomp.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libcomp.a $(TMPDIR)/compositions.o

sahatest: $(BINDIR)/sahatest
$(BINDIR)/sahatest: $(SRCDIR)/saha_test.cpp  $(LIBDIR)/libcomp.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/saha_test.cpp $(ESFLAGS) $(ESLIBS) -lcomp -llinerout -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/sahatest

genplot: $(BINDIR)/genplot
$(BINDIR)/genplot: $(SRCDIR)/genplot.cpp  $(XLIBSCHANGE) $(LIBDIR)/libplotutil.a
	$(CCOMP) $(CLFLAGS) -std=c++11 $(XMLINCLUDE) $(SRCDIR)/genplot.cpp $(PLOTUTILLIB) -lxio -lxstdlib -lxml2 -o $(BINDIR)/genplot

gentardis: $(BINDIR)/gentardis
$(BINDIR)/gentardis: $(SRCDIR)/gentardis.cpp $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gentardis.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxastro -lxio -lxstdlib -o $(BINDIR)/gentardis

gausstest: $(BINDIR)/gausstest
$(BINDIR)/gausstest: $(SRCDIR)/gaussian_fit_test.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h $(LIBDIR)/libmsdb.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gaussian_fit_test.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/gausstest

gather_pEW_vels: $(BINDIR)/gather_pEW_vels
$(BINDIR)/gather_pEW_vels: $(SRCDIR)/gather_pEW_vels.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gather_pEW_vels.cpp -lxmath -lxio -lxstdlib -o $(BINDIR)/gather_pEW_vels

test_msdb: $(BINDIR)/test_msdb
$(BINDIR)/test_msdb: $(SRCDIR)/msdb_test.cpp $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libmsdb.a
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/msdb_test.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/test_msdb

Vega_filters: $(BINDIR)/Vega_filters
$(BINDIR)/Vega_filters: $(SRCDIR)/Vega_filter_flux.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/Vega_filter_flux.cpp -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/Vega_filters

gather_photometry: $(BINDIR)/gather_photometry
$(BINDIR)/gather_photometry: $(SRCDIR)/gather_photometry.cpp $(XLIBSCHANGE)
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gather_photometry.cpp -lxio -lxstdlib -o $(BINDIR)/gather_photometry


gather_scalars: $(BINDIR)/gather_scalars
$(BINDIR)/gather_scalars: $(SRCDIR)/gather_scalars.cpp
	$(CCOMP) $(CLFLAGS) $(SRCDIR)/gather_scalars.cpp -o $(BINDIR)/gather_scalars

clean:
	-rm $(BINDIR)/*
	-rm $(TMPDIR)/*.*
	-rm $(LIBDIR)/*.a

