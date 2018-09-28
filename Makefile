# Makefile for proc map generation - for macOS

all:
	mkdir -p out/Test.app/Contents
	mkdir -p out/Test.app/Contents/MacOS
	mkdir -p out/Test.app/Contents/Resources
	cp ProcMapGeneration-macOS/Info_compiled.plist out/Test.app/Contents/Info.plist
	clang++ -O0 -std=c++11 -I/usr/local/include/irrlicht -L/usr/local/lib -lIrrlicht -framework OpenGL -framework AppKit -framework IOKit -framework Cocoa Shared/*.cpp XEffects/*.cpp -IShared -IXEffects -DMACOS -o out/Test.app/Contents/MacOS/ProceduralTerrainGeneration-macOS
	cp -R Shared/models out/Test.app/Contents/Resources/
	cp -R Shared/shader out/Test.app/Contents/Resources/
	cp -R Shared/fonts  out/Test.app/Contents/Resources/

clean:
	rm -rf out

