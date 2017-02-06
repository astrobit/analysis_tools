all: Plot_Utilities msdb photosphere reverse rlamc yaml2csv shex densprof quikplot quikplotspec flashtime userprof userseries gaussianprof quikplotseries equivwidth line_routines bestfitcsv combinedensdata ionabddet paperplot seriesewvmin gatherfits genfitmom modfits psfit psfitinter genfs min max data2databin ungatherfits tempex velev regenfits fixfits replot modelvelev diffusion flash2snec mve_vels multiion testeps libcomp sahatest genplot gentardis gausstest gather_pEW_vels test_msdb Vega_filters gather_photometry gather_scalars opmaptest gather_pstables 1dfm sf genjsonfit genmsdb gensingle ionphotcalc getopdata ionphotcalc57 gathertransitions gathertransitionsall statepoplib statepop velfntest
#spectrafit excluded (obsolete)
.PHONY: all

INCLUDEDIR=./include
SRCDIR=./src
BINDIR=./bin
TMPDIR=./obj
LIBDIR=./lib
CXXFLAGS+=-DMPICH_IGNORE_CXX_SEEK=1 -I$(INCLUDEDIR) --std=c++14 -fopenmp -c
CLFLAGS=-I$(INCLUDEDIR) -L$(LIBDIR) --std=c++14 -fopenmp 
LFLAGS=-DMPICH_IGNORE_CXX_SEEK=1 -I$(INCLUDEDIR) -L$(LIBDIR) -fopenmp
LIBCOMP=ar
LIBCOMPFLAG=-cvr
PLOTUTILLIB=-lplotutil
ESFLAGS= -fopenmp
ESLIBS= -les -lm -lcfitsio 
XLIBSPATH=~/xlibs
XLIBSCHANGE=$(XLIBSPATH)/lib/libxio.a $(XLIBSPATH)/lib/libxstdlib.a $(XLIBSPATH)/lib/libxmath.a $(XLIBSPATH)/lib/libxastro.a $(XLIBSPATH)/lib/libxflash.a
XMLINCLUDE= -I/usr/include/libxml2
ifdef TACC_HDF5_LIB
	LFLAGS += -L$(TACC_HDF5_LIB)
	CLFLAGS += -L$(TACC_HDF5_LIB)
endif

$(LIBDIR)/libplotutil.a: $(SRCDIR)/Plot_Utilities.cpp $(INCLUDEDIR)/Plot_Utilities.h $(XLIBSCHANGE) $(INCLUDEDIR)/eps_plot.h $(SRCDIR)/eps_plot.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/Plot_Utilities.cpp -o $(TMPDIR)/Plot_Utilities.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/eps_plot.cpp -o $(TMPDIR)/eps_plot.o
	-rm $(LIBDIR)/libplotutil.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libplotutil.a $(TMPDIR)/eps_plot.o $(TMPDIR)/Plot_Utilities.o 
Plot_Utilities: $(LIBDIR)/libplotutil.a

$(LIBDIR)/liblinerout.a: $(SRCDIR)/line_routines.cpp $(INCLUDEDIR)/line_routines.h $(XLIBSCHANGE) $(SRCDIR)/line_routines_gaussian_fit.cpp $(SRCDIR)/line_routines_ccm_dered.cpp  $(SRCDIR)/line_routines_radiation.cpp $(XLIBSCHANGE)
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_routines.cpp -o $(TMPDIR)/line_routines.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_routines_gaussian_fit.cpp -o $(TMPDIR)/line_routines_gaussian_fit.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_routines_ccm_dered.cpp -o $(TMPDIR)/line_routines_ccm_dered.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_routines_radiation.cpp -o $(TMPDIR)/line_routines_radiation.o
	-rm $(LIBDIR)/liblinerout.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/liblinerout.a $(TMPDIR)/line_routines.o $(TMPDIR)/line_routines_gaussian_fit.o $(TMPDIR)/line_routines_ccm_dered.o $(TMPDIR)/line_routines_radiation.o
line_routines: $(LIBDIR)/liblinerout.a

msdb: $(LIBDIR)/libmsdb.a
$(LIBDIR)/libmsdb.a: $(SRCDIR)/model_spectra_db.cpp $(INCLUDEDIR)/model_spectra_db.h
	$(CXX) $(CXXFLAGS) $(ESFLAGS) $(SRCDIR)/model_spectra_db.cpp -o $(TMPDIR)/model_spectra_db.o
	-rm $(LIBDIR)/libmsdb.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libmsdb.a $(TMPDIR)/model_spectra_db.o

$(OBJDIR)/line_anal.o: $(SRCDIR)/line_anal.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_anal.cpp  -o $(OBJDIR)/line_anal.o

$(OBJDIR)/line_analysis.o: $(SRCDIR)/line_analysis.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/line_analysis.cpp  -o $(OBJDIR)/line_analysis.o

$(BINDIR)/line_anal: $(OBJDIR)/line_anal.o $(OBJDIR)/line_analysis.o $(XLIBSCHANGE)
	$(CXX) $(LFLAGS) $(OBJDIR)/line_analysis.o $(OBJDIR)/line_anal.o  -lxmath -lxio -lxstdlib -lxastro -o $(BINDIR)/line_anal
line_anal: $(BINDIR)/line_anal

$(BINDIR)/lineanal2: $(SRCDIR)/line_analysis2.cpp $(XLIBSCHANGE)
	$(CXX) $(LFLAGS) $(SRCDIR)/line_analysis2.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -lhdf5 -lxflash -o $(BINDIR)/lineanal2
lineanal2: $(BINDIR)/lineanal2

$(BINDIR)/lineanal4: $(SRCDIR)/line_analysis4.cpp $(XLIBSCHANGE)
	$(CXX) $(LFLAGS) $(SRCDIR)/line_analysis4.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/lineanal4
lineanal4: $(BINDIR)/lineanal4

$(BINDIR)/lineanal5: $(SRCDIR)/line_analysis5.cpp $(XLIBSCHANGE)
	$(CXX) $(LFLAGS) $(SRCDIR)/line_analysis5.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/lineanal5
lineanal5: $(BINDIR)/lineanal5

$(BINDIR)/photosphere: $(SRCDIR)/photosphere.cpp $(XLIBSCHANGE)
	$(CXX) $(LFLAGS) $(SRCDIR)/photosphere.cpp -lxmath -lxio -lxstdlib -lxastro -lxmath -lhdf5 -lxflash -o $(BINDIR)/photosphere
photosphere: $(BINDIR)/photosphere

$(BINDIR)/reverse_line_anal: $(SRCDIR)/reverse_line_anal.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/reverse_line_anal.cpp -lyaml-cpp -lxio -lxstdlib -lxastro -lxmath -o $(BINDIR)/reverse_line_anal
reverse: $(BINDIR)/reverse_line_anal

$(BINDIR)/rlamc: $(SRCDIR)/rla_mass_combiner.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/rla_mass_combiner.cpp -lxio -lxstdlib -o $(BINDIR)/rlamc
rlamc: $(BINDIR)/rlamc

$(BINDIR)/yaml2csv: $(SRCDIR)/yaml2csv.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/yaml2csv.cpp -lyaml-cpp -o $(BINDIR)/yaml2csv
yaml2csv: $(BINDIR)/yaml2csv

$(BINDIR)/shex: $(SRCDIR)/extract_shell.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/extract_shell.cpp -lhdf5 -lxflash -lxio -lxstdlib -o $(BINDIR)/shex
shex: $(BINDIR)/shex

$(BINDIR)/densprof: $(SRCDIR)/dens_profiles.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/dens_profiles.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/densprof
densprof: $(BINDIR)/densprof

$(BINDIR)/quikplot: $(SRCDIR)/quikplot.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/quikplot.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/quikplot
quikplot: $(BINDIR)/quikplot

$(BINDIR)/quikplotspec: $(SRCDIR)/quikplotspectrum.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/quikplotspectrum.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/quikplotspec
quikplotspec: $(BINDIR)/quikplotspec

$(BINDIR)/flashtime: $(SRCDIR)/gettime.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/gettime.cpp -lhdf5 -lxflash -o $(BINDIR)/flashtime
flashtime: $(BINDIR)/flashtime

#$(BINDIR)/spectrafit: $(SRCDIR)/specta_fit.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
#	$(CXX) $(CLFLAGS) $(SRCDIR)/specta_fit.cpp  $(SRCDIR)/specta_fit_xfit.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/spectrafit
#spectrafit: $(BINDIR)/spectrafit


$(BINDIR)/userprof: $(SRCDIR)/user_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/user_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/userprof
userprof: $(BINDIR)/userprof

$(BINDIR)/userseries: $(LIBDIR)/libplotutil.a $(SRCDIR)/user_series.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/user_series.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/userseries
userseries: $(BINDIR)/userseries

$(BINDIR)/gaussianprof: $(SRCDIR)/gaussian_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/gaussian_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/gaussianprof
gaussianprof: $(BINDIR)/gaussianprof

quikplotseries: $(BINDIR)/quikplotseries 
$(BINDIR)/quikplotseries: $(SRCDIR)/quikplotseries.cpp $(LIBDIR)/libplotutil.a  $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/quikplotseries.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/quikplotseries

equivwidth: $(BINDIR)/equivwidth
$(BINDIR)/equivwidth: $(SRCDIR)/equivalentwidth.cpp $(LIBDIR)/liblinerout.a $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/equivalentwidth.cpp $(ESFLAGS) -llinerout -lxio -lxstdlib -lxmath $(ESLIBS) -o $(BINDIR)/equivwidth

bestfitcsv: $(BINDIR)/bestfitcsv
$(BINDIR)/bestfitcsv: $(SRCDIR)/bestfittocsv.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/bestfittocsv.cpp $(ESFLAGS) -lxio -lxstdlib $(ESLIBS) -o $(BINDIR)/bestfitcsv

combinedensdata: $(BINDIR)/combinedensdata
$(BINDIR)/combinedensdata: $(SRCDIR)/combinedensdata.cpp
	$(CXX) $(CLFLAGS) $(SRCDIR)/combinedensdata.cpp -o $(BINDIR)/combinedensdata

ionabddet: $(BINDIR)/ionabddet
$(BINDIR)/ionabddet: $(SRCDIR)/ion_abd_determination.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/ion_abd_determination.cpp -lhdf5 -lxflash -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/ionabddet

paperplot: $(BINDIR)/paperplot
$(BINDIR)/paperplot: $(LIBDIR)/libplotutil.a $(SRCDIR)/paper_plots.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/paper_plots.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/paperplot

seriesewvmin: $(BINDIR)/seriesewvmin
$(BINDIR)/seriesewvmin: $(LIBDIR)/libplotutil.a $(SRCDIR)/paper_plots.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/ES_Vel_min.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/seriesewvmin

gatherfits: $(BINDIR)/gatherfits
$(BINDIR)/gatherfits: $(SRCDIR)/gather_best_fit_data.cpp $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/gather_best_fit_data.cpp $(ESFLAGS)  $(ESLIBS) -lxstdlib -o $(BINDIR)/gatherfits

genfitmom: $(BINDIR)/genfitmom
$(BINDIR)/genfitmom: $(SRCDIR)/generate_fit_moments.cpp $(LIBDIR)/liblinerout.a $(XLIBSCHANGE) $(SRCDIR)/eps_plot.cpp
	$(CXX) $(CLFLAGS) $(SRCDIR)/generate_fit_moments.cpp $(SRCDIR)/eps_plot.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/genfitmom

modfits: $(BINDIR)/modfits
$(BINDIR)/modfits: $(SRCDIR)/mod_best_fit_file.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/mod_best_fit_file.cpp $(ESFLAGS) $(ESLIBS) -lxstdlib -o $(BINDIR)/modfits

$(BINDIR)/psfit: $(SRCDIR)/PS_temp.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/PS_temp.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/psfit
psfit: $(BINDIR)/psfit

$(BINDIR)/psfitinter: $(SRCDIR)/PS_temp_inter.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/PS_temp_inter.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/psfitinter
psfitinter: $(BINDIR)/psfitinter

genfs: $(BINDIR)/genfs
$(BINDIR)/genfs: $(SRCDIR)/generate_flattened_spectra.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/generate_flattened_spectra.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/genfs

min: $(BINDIR)/min
$(BINDIR)/min: $(SRCDIR)/min.cpp
	$(CXX) $(CLFLAGS) $(SRCDIR)/min.cpp -o $(BINDIR)/min

max: $(BINDIR)/max
$(BINDIR)/max: $(SRCDIR)/max.cpp
	$(CXX) $(CLFLAGS) $(SRCDIR)/max.cpp -o $(BINDIR)/max

data2databin: $(BINDIR)/data2databin
$(BINDIR)/data2databin: $(SRCDIR)/data2databin.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/data2databin.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/data2databin

#data2csv: $(BINDIR)/data2csv
#$(BINDIR)/data2csv: $(SRCDIR)/data2csv.cpp $(INCLUDEDIR)/best_fit_data.h
#	$(CXX) $(CLFLAGS) $(SRCDIR)/data2csv.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/data2csv

#csv2data: $(BINDIR)/csv2data
#$(BINDIR)/csv2data: $(SRCDIR)/csv2data.cpp $(INCLUDEDIR)/best_fit_data.h
#	$(CXX) $(CLFLAGS) $(SRCDIR)/csv2data.cpp $(ESFLAGS) $(ESLIBS) -lxio -lxstdlib -o $(BINDIR)/csv2data

ungatherfits: $(BINDIR)/ungatherfits
$(BINDIR)/ungatherfits: $(SRCDIR)/ungather_best_fit_data.cpp $(INCLUDEDIR)/best_fit_data.h $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/ungather_best_fit_data.cpp $(ESFLAGS)  $(ESLIBS) -lxstdlib -o $(BINDIR)/ungatherfits

tempex: $(BINDIR)/tempex
$(BINDIR)/tempex: $(SRCDIR)/temp_extractor.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/temp_extractor.cpp  -lhdf5 -lxflash -o $(BINDIR)/tempex

$(BINDIR)/velev: $(SRCDIR)/nofit_user_profile.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/nofit_user_profile.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/velev
velev: $(BINDIR)/velev


$(BINDIR)/regenfits: $(SRCDIR)/regen_data_file.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/regen_data_file.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/regenfits
regenfits: $(BINDIR)/regenfits

$(BINDIR)/fixfits: $(SRCDIR)/fix_data_file.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/fix_data_file.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/fixfits
fixfits: $(BINDIR)/fixfits

$(BINDIR)/replot: $(SRCDIR)/replot.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/replot.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/replot
replot: $(BINDIR)/replot

modelvelev:$(BINDIR)/modelvelev
$(BINDIR)/modelvelev: $(SRCDIR)/model_vel_evolution.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h $(LIBDIR)/libmsdb.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/model_vel_evolution.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxastro -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/modelvelev

diffusion: $(BINDIR)/diffusion
$(BINDIR)/diffusion: $(SRCDIR)/diffusion.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/diffusion.cpp -lxmath -lxio -lxstdlib -o $(BINDIR)/diffusion

flash2snec: $(BINDIR)/flash2snec
$(BINDIR)/flash2snec: $(SRCDIR)/FLASH_2_SNEC.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/FLASH_2_SNEC.cpp -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/flash2snec

mve_vels: $(BINDIR)/mve_vels
$(BINDIR)/mve_vels: $(SRCDIR)/mve_velocity_plot.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/mve_velocity_plot.cpp -lxio -lxstdlib -o $(BINDIR)/mve_vels

multiion: $(BINDIR)/multiion
$(BINDIR)/multiion: $(SRCDIR)/multiion_spectra.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libcomp.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/multiion_spectra.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -lcomp -llinerout -lxmath -lxastro -lxio -lxstdlib -o $(BINDIR)/multiion

testeps: $(BINDIR)/testeps
$(BINDIR)/testeps: $(SRCDIR)/testeps.cpp  $(LIBDIR)/libplotutil.a $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/testeps.cpp $(ESFLAGS) $(PLOTUTILLIB) -lxio -lxstdlib -o $(BINDIR)/testeps

libcomp: $(LIBDIR)/libcomp.a
$(LIBDIR)/libcomp.a: $(SRCDIR)/compositions.cpp $(INCLUDEDIR)/compositions.h  $(XLIBSCHANGE)
	$(CXX) $(CXXFLAGS) $(ESFLAGS) $(SRCDIR)/compositions.cpp -o $(TMPDIR)/compositions.o
	-rm $(LIBDIR)/libcomp.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libcomp.a $(TMPDIR)/compositions.o

sahatest: $(BINDIR)/sahatest
$(BINDIR)/sahatest: $(SRCDIR)/saha_test.cpp  $(LIBDIR)/libcomp.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/saha_test.cpp $(ESFLAGS) $(ESLIBS) -lcomp -llinerout -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/sahatest

genplot: $(BINDIR)/genplot
$(BINDIR)/genplot: $(SRCDIR)/genplot.cpp  $(XLIBSCHANGE) $(LIBDIR)/libplotutil.a
	$(CXX) $(CLFLAGS) -std=c++11 $(XMLINCLUDE) $(SRCDIR)/genplot.cpp $(PLOTUTILLIB) -lxio -lxstdlib -lxml2 -o $(BINDIR)/genplot

gentardis: $(BINDIR)/gentardis
$(BINDIR)/gentardis: $(SRCDIR)/gentardis.cpp $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/gentardis.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxastro -lxio -lxstdlib -o $(BINDIR)/gentardis

gausstest: $(BINDIR)/gausstest
$(BINDIR)/gausstest: $(SRCDIR)/gaussian_fit_test.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/best_fit_data.h $(LIBDIR)/libmsdb.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/gaussian_fit_test.cpp $(ESFLAGS) $(PLOTUTILLIB) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/gausstest

gather_pEW_vels: $(BINDIR)/gather_pEW_vels
$(BINDIR)/gather_pEW_vels: $(SRCDIR)/gather_pEW_vels.cpp $(XLIBSCHANGE)  $(LIBDIR)/liblinerout.a
	$(CXX)  $(ESFLAGS) $(CLFLAGS) $(SRCDIR)/gather_pEW_vels.cpp  $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/gather_pEW_vels

test_msdb: $(BINDIR)/test_msdb
$(BINDIR)/test_msdb: $(SRCDIR)/msdb_test.cpp $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libmsdb.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/msdb_test.cpp $(ESFLAGS) $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -lmsdb -o $(BINDIR)/test_msdb

Vega_filters: $(BINDIR)/Vega_filters
$(BINDIR)/Vega_filters: $(SRCDIR)/Vega_filter_flux.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/Vega_filter_flux.cpp -lxastro -lxmath -lxio -lxstdlib -o $(BINDIR)/Vega_filters

gather_photometry: $(BINDIR)/gather_photometry
$(BINDIR)/gather_photometry: $(SRCDIR)/gather_photometry.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/gather_photometry.cpp -lxio -lxstdlib -o $(BINDIR)/gather_photometry


gather_scalars: $(BINDIR)/gather_scalars
$(BINDIR)/gather_scalars: $(SRCDIR)/gather_scalars.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/gather_scalars.cpp -lxio -lxstdlib -lxflash -lhdf5 -o $(BINDIR)/gather_scalars

opmaptest: $(BINDIR)/opmaptest
$(BINDIR)/opmaptest: $(SRCDIR)/opacity_map_test.cpp $(LIBDIR)/libplotutil.a $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/opacity_map_test.cpp  -llinerout -lxio -lxstdlib -o $(BINDIR)/opmaptest

gather_pstables: $(BINDIR)/gather_pstables
$(BINDIR)/gather_pstables: $(SRCDIR)/gather_photospheres.cpp $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(SRCDIR)/gather_photospheres.cpp -lxio -lxstdlib -o $(BINDIR)/gather_pstables

1dfm: $(BINDIR)/1dfm
$(BINDIR)/1dfm: $(SRCDIR)/1dFlashMovie.cpp $(XLIBSCHANGE) $(LIBDIR)/libplotutil.a
	$(CXX) $(CLFLAGS) $(SRCDIR)/1dFlashMovie.cpp $(PLOTUTILLIB) -lxio -lxstdlib -lhdf5 -lxflash -o $(BINDIR)/1dfm

ifdef JSONCPP
sf: $(BINDIR)/sf
$(LIBDIR)/libsf.a: $(SRCDIR)/sf_add_model_to_list.cpp $(SRCDIR)/sf_bracket_temp.cpp $(SRCDIR)/sf_calc_observables.cpp $(SRCDIR)/sf_continuum_fit.cpp $(SRCDIR)/sf_deredden.cpp $(SRCDIR)/sf_fit_function.cpp $(SRCDIR)/sf_get_fit.cpp $(SRCDIR)/sf_get_norm_flux.cpp $(SRCDIR)/sf_inc_index.cpp $(SRCDIR)/sf_load_data.cpp $(SRCDIR)/sf_load_data_files.cpp $(SRCDIR)/sf_load_models.cpp $(SRCDIR)/sf_msdb_load_gen.cpp $(SRCDIR)/sf_output_results.cpp $(SRCDIR)/sf_parse_xml.cpp $(SRCDIR)/sf_perform_fits.cpp $(SRCDIR)/sf_pew_fit.cpp $(SRCDIR)/sf_validate_json_data.cpp $(SRCDIR)/sf_write_fit.cpp $(SRCDIR)/sf_write_target_fit.cpp   $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libmsdb.a $(SRCDIR)/spectra_fit_genfits.cpp $(SRCDIR)/spectra_fit_fit_results.cpp
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_add_model_to_list.cpp -o $(TMPDIR)/sf_add_model_to_list.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_bracket_temp.cpp -o $(TMPDIR)/sf_bracket_temp.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_calc_observables.cpp -o $(TMPDIR)/sf_calc_observables.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_continuum_fit.cpp -o $(TMPDIR)/sf_continuum_fit.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_deredden.cpp -o $(TMPDIR)/sf_deredden.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_fit_function.cpp -o $(TMPDIR)/sf_fit_function.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_get_fit.cpp -o $(TMPDIR)/sf_get_fit.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_get_norm_flux.cpp -o $(TMPDIR)/sf_get_norm_flux.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_inc_index.cpp -o $(TMPDIR)/sf_inc_index.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_load_data.cpp -o $(TMPDIR)/sf_load_data.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_load_data_files.cpp -o $(TMPDIR)/sf_load_data_files.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_load_models.cpp -o $(TMPDIR)/sf_load_models.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_msdb_load_gen.cpp -o $(TMPDIR)/sf_msdb_load_gen.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_output_results.cpp -o $(TMPDIR)/sf_output_results.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_parse_xml.cpp -o $(TMPDIR)/sf_parse_xml.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_perform_fits.cpp -o $(TMPDIR)/sf_perform_fits.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_pew_fit.cpp -o $(TMPDIR)/sf_pew_fit.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_validate_json_data.cpp -o $(TMPDIR)/sf_validate_json_data.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_write_fit.cpp -o $(TMPDIR)/sf_write_fit.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/sf_write_target_fit.cpp -o $(TMPDIR)/sf_write_target_fit.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/spectra_fit_genfits.cpp -o $(TMPDIR)/spectra_fit_genfits.o
	$(CXX) $(CXXFLAGS) $(XMLINCLUDE) $(SRCDIR)/spectra_fit_fit_results.cpp -o $(TMPDIR)/spectra_fit_fit_results.o
	-rm $(LIBDIR)/libsf.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libsf.a $(TMPDIR)/sf_add_model_to_list.o  $(TMPDIR)/sf_bracket_temp.o  $(TMPDIR)/sf_calc_observables.o  $(TMPDIR)/sf_continuum_fit.o $(TMPDIR)/sf_deredden.o $(TMPDIR)/sf_fit_function.o $(TMPDIR)/sf_get_fit.o $(TMPDIR)/sf_get_norm_flux.o $(TMPDIR)/sf_inc_index.o $(TMPDIR)/sf_load_data.o $(TMPDIR)/sf_load_data_files.o $(TMPDIR)/sf_load_models.o $(TMPDIR)/sf_msdb_load_gen.o $(TMPDIR)/sf_output_results.o $(TMPDIR)/sf_parse_xml.o $(TMPDIR)/sf_perform_fits.o $(TMPDIR)/sf_pew_fit.o $(TMPDIR)/sf_validate_json_data.o $(TMPDIR)/sf_write_fit.o $(TMPDIR)/sf_write_target_fit.o $(TMPDIR)/spectra_fit_genfits.o $(TMPDIR)/spectra_fit_fit_results.o

$(BINDIR)/sf: $(SRCDIR)/spectra_fit.1.0.cpp  $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a $(LIBDIR)/libmsdb.a $(LIBDIR)/libsf.a $(INCLUDEDIR)/eps_plot.h  $(INCLUDEDIR)/specfit.h
	$(CXX) $(CLFLAGS) $(XMLINCLUDE) $(SRCDIR)/spectra_fit.1.0.cpp $(JSONCPP) $(ESFLAGS) -lsf $(ESLIBS) $(PLOTUTILLIB) -llinerout -lxml2 -lxmath -lxastro  -lxio -lxstdlib -lmsdb -o $(BINDIR)/sf

genjsonfit: $(BINDIR)/genjsonfit
$(BINDIR)/genjsonfit: $(SRCDIR)/gen_json_fit_list.cpp  $(XLIBSCHANGE)
	$(CXX) $(CLFLAGS) $(XMLINCLUDE) $(SRCDIR)/gen_json_fit_list.cpp $(JSONCPP) -lxastro -lxmath -lxstdlib -o $(BINDIR)/genjsonfit
else
sf: $(BINDIR)/sf
genjsonfit: $(BINDIR)/sf
$(BINDIR)/sf: $(SRCDIR)/spectra_fit.1.0.cpp $(XLIBSCHANGE) $(LIBDIR)/libspecfit.a $(LIBDIR)/liblinerout.a $(LIBDIR)/libmsdb.a $(SRCDIR)/spectra_fit_genfits.cpp  $(LIBDIR)/libmsdb.a
	@echo "Symbol JSONCPP not found. Make sure that jsoncpp is installed and the JSONCPP variable points to the path to jsoncpp.cpp"
endif

genmsdb: $(BINDIR)/genmsdb
$(BINDIR)/genmsdb: $(SRCDIR)/gen_msdb.cpp 
	$(CXX)  $(ESFLAGS) $(CLFLAGS) $(SRCDIR)/gen_msdb.cpp $(ESLIBS) -lmsdb -o $(BINDIR)/genmsdb

gensingle: $(BINDIR)/gensingle
$(BINDIR)/gensingle: $(SRCDIR)/gensingle.cpp $(XLIBSCHANGE) $(LIBDIR)/liblinerout.a
	$(CXX)  $(ESFLAGS) $(CLFLAGS) $(SRCDIR)/gensingle.cpp $(ESLIBS) -llinerout -lxmath -lxio -lxstdlib -o $(BINDIR)/gensingle

ionphotcalc: $(BINDIR)/ionphotcalc
$(BINDIR)/ionphotcalc: $(SRCDIR)/ionphotcalc.cpp $(XLIBSCHANGE)
	$(CXX)	$(CLFLAGS) $(SRCDIR)/ionphotcalc.cpp -lxmath -lxastro -o $(BINDIR)/ionphotcalc

ionphotcalc57: $(BINDIR)/ionphotcalc57
$(BINDIR)/ionphotcalc57: $(SRCDIR)/ionphotcalc_m57.cpp $(XLIBSCHANGE)
	$(CXX)	$(CLFLAGS) $(SRCDIR)/ionphotcalc_m57.cpp -lxmath -lxastro -o $(BINDIR)/ionphotcalc57


getopdata: $(BINDIR)/getopdata
$(BINDIR)/getopdata: $(SRCDIR)/get_opdata.cpp
	$(CXX)	$(CLFLAGS) $(SRCDIR)/get_opdata.cpp -o $(BINDIR)/getopdata

gathertransitions: $(BINDIR)/gathertransitions
$(BINDIR)/gathertransitions: $(SRCDIR)/gather_transitions.cpp $(XLIBSCHANGE)
	$(CXX)	$(CLFLAGS) $(SRCDIR)/gather_transitions.cpp -lxio -lxstdlib -o $(BINDIR)/gathertransitions

gathertransitionsall: $(BINDIR)/gathertransitionsall
$(BINDIR)/gathertransitionsall: $(SRCDIR)/gather_transitions_all.cpp $(XLIBSCHANGE)
	$(CXX)	$(CLFLAGS) $(SRCDIR)/gather_transitions_all.cpp -lxio -lxstdlib -o $(BINDIR)/gathertransitionsall


$(LIBDIR)/libsp.a: $(SRCDIR)/state_pop_lib_cursor.cpp $(INCLUDEDIR)/state_pops.h $(XLIBSCHANGE) $(SRCDIR)/state_pop_lib_ang_mom.cpp $(SRCDIR)/state_pop_lib_get_def_cfg.cpp  $(SRCDIR)/state_pop_lib_prt_cfg_vect.cpp  $(SRCDIR)/state_pop_lib_read_K_state.cpp  $(SRCDIR)/state_pop_lib_read_OP_state.cpp $(SRCDIR)/state_pop_lib_ion_rec_eqv.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_cursor.cpp -o $(TMPDIR)/state_pop_lib_cursor.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_ang_mom.cpp -o $(TMPDIR)/state_pop_lib_ang_mom.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_get_def_cfg.cpp -o $(TMPDIR)/state_pop_lib_get_def_cfg.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_prt_cfg_vect.cpp -o $(TMPDIR)/state_pop_lib_prt_cfg_vect.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_read_K_state.cpp -o $(TMPDIR)/state_pop_lib_read_K_state.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_read_OP_state.cpp -o $(TMPDIR)/state_pop_lib_read_OP_state.o
	$(CXX) $(CXXFLAGS) $(SRCDIR)/state_pop_lib_ion_rec_eqv.cpp -o $(TMPDIR)/state_pop_lib_ion_rec_eqv.o
	-rm $(LIBDIR)/libsp.a
	$(LIBCOMP) $(LIBCOMPFLAG) $(LIBDIR)/libsp.a $(TMPDIR)/state_pop_lib_cursor.o $(TMPDIR)/state_pop_lib_ang_mom.o $(TMPDIR)/state_pop_lib_get_def_cfg.o $(TMPDIR)/state_pop_lib_prt_cfg_vect.o $(TMPDIR)/state_pop_lib_read_K_state.o $(TMPDIR)/state_pop_lib_read_OP_state.o $(TMPDIR)/state_pop_lib_ion_rec_eqv.o
statepoplib: $(LIBDIR)/libsp.a

statepop: $(BINDIR)/statepop
$(BINDIR)/statepop: $(SRCDIR)/state_pops.cpp $(XLIBSCHANGE) $(LIBDIR)/libsp.a $(LIBDIR)/liblinerout.a $(INCLUDEDIR)/kurucz_data.h $(INCLUDEDIR)/radiation.h $(INCLUDEDIR)/opacity_project_pp.h  $(INCLUDEDIR)/velocity_function.h
	$(CXX) $(CLFLAGS) -I$(HOME)/arpack++/include $(SRCDIR)/state_pops.cpp -lsp -llinerout -lxastro -lxmath -lxio -lxstdlib -larpack -lsuperlu -lblas -llapack -o $(BINDIR)/statepop

velfntest: $(BINDIR)/velfntest
$(BINDIR)/velfntest: $(SRCDIR)/velfntest.cpp $(XLIBSCHANGE) $(INCLUDEDIR)/velocity_function.h
	$(CXX) $(CLFLAGS) $(SRCDIR)/velfntest.cpp -lxastro -lxmath -o $(BINDIR)/velfntest

clean:
	-rm $(BINDIR)/*
	-rm $(TMPDIR)/*.*
	-rm $(LIBDIR)/*.a

