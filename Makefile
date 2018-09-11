# Makefile for proc map generation - for macOS

all:
	clang++ -O0 -std=c++11 -I/usr/local/include/irrlicht -L/usr/local/lib -lIrrlicht -framework OpenGL -framework AppKit -framework IOKit -framework Cocoa ProcMapGeneration/*.cpp -DMACOS
