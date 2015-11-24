Runs on windows
On windows (codeblocks):
Add communication.h, communication.cpp, hokuyoTest to project
In Settings > Compiler > Linker Settings > Other Linker options, add the following two lines

-lwinmm
-lwsock32


Click 'Build and run'

On unix:
g++ hokuyoTest.cpp communication.cpp -o hokuyo
On mac:
clang++ hokuyoTest.cpp communication.cpp -o hokuyo

To run on unix/mac:
./hokuyo

Will create a text file 'tcp-log.txt in the same folder that hokuyo is run in