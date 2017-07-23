cmake_minimum_required(VERSION 2.6)
project(VoxModSynth)

include_directories(
  ./
  ${PROJECT_SOURCE_DIR}/libsl-small
  ${PROJECT_SOURCE_DIR}/libsl-small/LibSL
)

link_directories(
	${PROJECT_SOURCE_DIR}/
  ${LIBSL_BINARY_DIR}
)

add_definitions(-DSRC_PATH=\"${CMAKE_SOURCE_DIR}/\")

SET(SOURCES
  main.cpp
  libsl-small/LibSL/Math/Math.cpp
  libsl-small/LibSL/Math/Vertex.cpp
  libsl-small/LibSL/System/System.cpp
  libsl-small/LibSL/CppHelpers/CppHelpers.cpp
)

add_executable(VoxModSynth ${SOURCES})

if(WIN32)
target_link_libraries(VoxModSynth shlwapi)
endif(WIN32)
