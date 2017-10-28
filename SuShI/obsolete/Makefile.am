AUTOMAKE_OPTIONS = foreign subdir-objects

noinst_PROGRAMS = line_anal lineanal2 lineanal4 lineanal5 reverse_line_anal rlamc userprof userseries quikplotseries bestfitcsv combinedensdata ionabddet paperplot seriesewvmin gatherfits genfitmom modfits psfit psfitinter genfs ungatherfits data2databin data2csv csv2data tempex velev regenfits fixfits replot diffusion gather_pEW_vels gather_scalars gausstest

AM_CXXFLAGS = -I${srcdir}/../include -I${srcdir}/./include

line_anal_SOURCES = src/line_analysis.cpp src/line_anal.cpp
line_anal_LDADD = ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS}
line_anal_CXXFLAGS = ${xmath_LIBS} ${xioCFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS}

lineanal2_SOURCES = src/line_analysis2.cpp
lineanal2_LDADD = ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS} ${xmath_LIBS} -lhdf5 ${xflash_LIBS}
lineanal2_CXXFLAGS = ${xmath_CFLAGS} ${xioCFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS} ${xflash_CFLAGS}

lineanal4_SOURCES = src/line_analysis4.cpp
lineanal4_LDADD = ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS} ${xmath_LIBS}
lineanal4_CXXFLAGS = ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS}

lineanal5_SOURCES = src/line_analysis5.cpp
lineanal5_LDADD = ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS} ${xmath_LIBS}
lineanal5_CXXFLAGS = ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS}

reverse_line_anal_SOURCES = src/reverse_line_anal.cpp
reverse_line_anal_LDADD = ${yamlcpp_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${xastro_LIBS} ${xmath_LIBS}
reverse_line_anal_CXXFLAGS = ${yamlcpp_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS}

rlamc_SOURCES = src/rla_mass_combiner.cpp
rlamc_LDADD = ${xio_LIBS} ${xstdlib_LIBS}
rlamc_CXXFLAGS = ${xio_CFLAGS} ${xstdlib_CFLAGS}

userprof_SOURCES = src/user_profile.cpp
userprof_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} 
userprof_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_LIBS} ${xio_CFLAGS} ${xstdlib_CFLAGS} 

userseries_SOURCES = src/user_series.cpp
userseries_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} 
userseries_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} 

quikplotseries_SOURCES = src/quikplotseries.cpp
quikplotseries_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
quikplotseries_CXXFLAGS = -I${top_srcdir}/esplot/include ${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} ${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

bestfitcsv_SOURCES = src/bestfittocsv.cpp
bestfitcsv_LDADD =  ${xio_LIBS} ${xstdlib_LIBS} ${es_LIBS} ${cfitsio_LIBS}
bestfitcsv_CXXFLAGS =  ${xio_CFLAGS} ${xstdlib_CFLAGS} ${es_CFLAGS} ${cfitsio_CFLAGS}

combinedensdata_SOURCES = src/combinedensdata.cpp

ionabddet_SOURCES = src/ion_abd_determination.cpp
ionabddet_LDADD =  -lhdf5 ${xflash_LIBS} ${xastro_LIBS} ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
ionabddet_CXXFLAGS =  ${xflash_CFLAGS} ${xastro_CFLAGS} ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

paperplot_SOURCES = src/paper_plots.cpp
paperplot_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
paperplot_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

seriesewvmin_SOURCES = src/ES_Vel_min.cpp
seriesewvmin_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
seriesewvmin_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

gatherfits_SOURCES = src/gather_best_fit_data.cpp
gatherfits_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xstdlib_LIBS}
gatherfits_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xstdlib_CFLAGS}

genfitmom_SOURCES = src/generate_fit_moments.cpp
genfitmom_LDADD =  ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
genfitmom_CXXFLAGS =  ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

modfits_SOURCES = src/mod_best_fit_file.cpp
modfits_LDADD =  ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${xstdlib_LIBS}
modfits_CXXFLAGS =  ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${xstdlib_CFLAGS}

psfit_SOURCES = src/PS_temp.cpp
psfit_LDADD =   ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
psfit_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

psfitinter_SOURCES = src/PS_temp_inter.cpp
psfitinter_LDADD =   ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
psfitinter_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

genfs_SOURCES = src/generate_flattened_spectra.cpp
genfs_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
genfs_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

ungatherfits_SOURCES = src/ungather_best_fit_data.cpp
ungatherfits_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xstdlib_LIBS}
ungatherfits_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xstdlib_CFLAGS}

data2databin_SOURCES = src/data2databin.cpp
data2databin_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
data2databin_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

data2csv_SOURCES = src/data2csv.cpp
data2csv_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
data2csv_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

csv2data_SOURCES = src/csv2data.cpp
csv2data_LDADD = ${es_LIBS} ${cfitsio_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
csv2data_CXXFLAGS = ${es_CFLAGS} ${cfitsio_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

tempex_SOURCES = src/temp_extractor.cpp
tempex_LDADD = -lhdf5 ${xflash_LIBS}
tempex_CXXFLAGS = -lhdf5 ${xflash_CFLAGS}

velev_SOURCES = src/nofit_user_profile.cpp
velev_LDADD =  ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
velev_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

regenfits_SOURCES = src/regen_data_file.cpp
regenfits_LDADD =  ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
regenfits_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

fixfits_SOURCES = src/fix_data_file.cpp
fixfits_LDADD =  ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
fixfits_CXXFLAGS =  -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

replot_SOURCES = src/replot.cpp
replot_LDADD =  ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
replot_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

diffusion_SOURCES = src/diffusion.cpp
diffusion_LDADD =   ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS}
diffusion_CXXFLAGS =   ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS}

gather_pEW_vels_SOURCES = src/gather_pEW_vels.cpp
gather_pEW_vels_LDADD =    ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} 
gather_pEW_vels_CXXFLAGS =    ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} 

gather_scalars_SOURCES = src/gather_scalars.cpp
gather_scalars_LDADD =  ${xio_LIBS} ${xstdlib_LIBS} ${xflash_LIBS} -lhdf5
gather_scalars_CXXFLAGS =  ${xio_CFLAGS} ${xstdlib_CFLAGS} ${xflash_CFLAGS}

gausstest_SOURCES = src/gaussian_fit_test.cpp
gausstest_LDADD = ${top_srcdir}/esplot/libesplot.a ${top_srcdir}/epsplot/libepsplot.a ${es_LIBS} ${cfitsio_LIBS} ${top_srcdir}/linerout/liblinerout.a ${xmath_LIBS} ${xio_LIBS} ${xstdlib_LIBS} ${top_srcdir}/msdb/libmsdb.a
gausstest_CXXFLAGS = -I${top_srcdir}/esplot/include -I${top_srcdir}/epsplot/include ${es_CFLAGS} ${cfitsio_CFLAGS} -I${top_srcdir}/linerout/include ${xmath_CFLAGS} ${xio_CFLAGS} ${xstdlib_CFLAGS} -I${top_srcdir}/msdb/include

