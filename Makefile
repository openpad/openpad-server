all:
	g++ -std=c++11 -I./PracticalSocket -I./Example -I./rapidjson -I./rapidjson/internal -I./rapidjson/error \
	-I./openpad openpad/*.cpp PracticalSocket/*.cpp \
	Example/*.cpp main.cpp -o ServerExec

java:
	swig -c++ -java -package com.openpad.server Java\ Wrapper/openpad.i
	g++ -std=c++11 -c -I./PracticalSocket -I./Examples -I./json \
	-I./openpad -I/Library/Java/JavaVirtualMachines/jdk1.8.0_25.jdk/Contents/Home/include \
	-I/Library/Java/JavaVirtualMachines/jdk1.8.0_25.jdk/Contents/Home/include/darwin openpad/*.cpp PracticalSocket/*.cpp \
	Example/*.cpp Java\ Wrapper/*.cxx -Wall -fPIC
	g++ -dynamiclib *.o -L. -ljsoncpp -o ./Build/libopenpad.jnilib

clean:
	rm *.o
