all:
	g++ -std=c++11 -I./PracticalSocket -I./Example -I./rapidjson -I./rapidjson/internal -I./rapidjson/error \
	-I./openpad openpad/*.cpp PracticalSocket/*.cpp \
	Example/*.cpp main.cpp -o ServerExec

java:
	swig -c++ -java -package com.openpad.server Java\ Wrapper/openpad.i
	g++ -std=c++11 -c -I./PracticalSocket -I./Examples -I./rapidjson -I./rapidjson/internal -I./rapidjson/error \
	-I./PlayPhone -I/Library/Java/JavaVirtualMachines/jdk1.7.0_45.jdk/Contents/Home/include \
	-I/Library/Java/JavaVirtualMachines/jdk1.7.0_45.jdk/Contents/Home/include/darwin PlayPhone/*.cpp PracticalSocket/*.cpp \
	Example/*.cpp Java\ Wrapper/*.cxx -Wall -fPIC
	g++ -dynamiclib *.o -o ./Build/libopenpad.jnilib

clean:
	rm *.o
