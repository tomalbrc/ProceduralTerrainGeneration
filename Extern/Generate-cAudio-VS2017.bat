cd cAudio/CMake
mkdir build32 & pushd build32
cmake -DCAUDIO_DEPENDENCIES_DIR=../../Dependencies -G "Visual Studio 15 2017" ../..
popd
mkdir build64 & pushd build64
cmake -DCAUDIO_DEPENDENCIES_DIR=../../Dependencies64 -G "Visual Studio 15 2017 Win64" ../..
popd
cmake --config Release -DCAUDIO_DEPENDENCIES_DIR=../Dependencies64 --build build64
cmake --config Release -DCAUDIO_DEPENDENCIES_DIR=../Dependencies --build build32
pause
