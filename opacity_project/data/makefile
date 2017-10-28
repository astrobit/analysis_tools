all: quikread config_reprocess

quikread: quikread.cpp ../../include/opacity_project_pp.h
	g++ -I../../include quikread.cpp -lxastro -lxstdlib -o quikread

config_reprocess: config_reprocess.cpp
	g++ config_reprocess.cpp -o config_reprocess


