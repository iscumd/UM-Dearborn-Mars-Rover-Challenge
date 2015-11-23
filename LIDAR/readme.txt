Doesn't run on windows computers yet
On unix:
g++ hokuyoTest.cpp communication.cpp -o hokuyo
On mac:
clang hokuyoTest.cpp communication.cpp -o hokuyo

To run on unix/mac:
./hokuyo

Will create a text file 'tcp-log.txt in the same folder that hokuyo is run in