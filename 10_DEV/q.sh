#!bash
/usr/bin/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/home/david/Desktop/Sandbox-Depthwatch -B/home/david/Desktop/Sandbox-Depthwatch/build -G "Unix Makefiles"
cd build
make
