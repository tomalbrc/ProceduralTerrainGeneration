cd bullet3/build3
mkdir build32 & pushd build32
cmake -G "Visual Studio 15 2017" ../..
popd
mkdir build64 & pushd build64
cmake -G "Visual Studio 15 2017 Win64" ../..
popd
cmake --config Release -DBUILD_CPU_DEMOS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_CLSOCKET=OFF -DBUILD_ENET=OFF -DBUILD_EXTRAS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF --build build64
cmake --config Release -DBUILD_CPU_DEMOS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_CLSOCKET=OFF -DBUILD_ENET=OFF -DBUILD_EXTRAS=OFF -DBUILD_OPENGL3_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF --build build32
pause
