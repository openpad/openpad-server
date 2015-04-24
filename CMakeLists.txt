cmake_minimum_required (VERSION 2.8)
project (openpad)

option(BUILD_EXAMPLE "Build an example server" OFF)

include_directories(
	${CMAKE_SOURCE_DIR}/PracticalSocket
	${CMAKE_SOURCE_DIR}/json
	${CMAKE_SOURCE_DIR}/openpad
)

SET( HEADERS 
	PracticalSocket/PracticalSocket.h
	openpad/openpad.h
	openpad/Request.h
	openpad/Server.h
	openpad/ServerHandler.h
	openpad/Util.h
	json/json.h
	Example/DebugGame.h
	)

set( SOURCES
	PracticalSocket/PracticalSocket.cpp
	openpad/Request.cpp
	openpad/Server.cpp
	openpad/ServerHandler.cpp
	openpad/Util.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

add_library(openpad_static ${HEADERS} ${SOURCES} )
add_library(openpad_shared SHARED ${HEADERS} ${SOURCES} )
target_link_libraries(openpad_static ${CMAKE_SOURCE_DIR}/libjsoncpp.a )
target_link_libraries(openpad_shared ${CMAKE_SOURCE_DIR}/libjsoncpp.a )

if(BUILD_EXAMPLE)
	add_subdirectory(Example)
ENDIF(BUILD_EXAMPLE)
