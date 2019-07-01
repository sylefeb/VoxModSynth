cmake_minimum_required(VERSION 2.6)
project(VoxModSynth)

include_directories(
  ./
  ${PROJECT_SOURCE_DIR}/LibSL-small
  ${PROJECT_SOURCE_DIR}/LibSL-small/src/
  ${PROJECT_SOURCE_DIR}/LibSL-small/src/LibSL  
)

link_directories(
	${PROJECT_SOURCE_DIR}/
  ${LIBSL_BINARY_DIR}
)

add_definitions(-DSRC_PATH=\"${CMAKE_SOURCE_DIR}/\")

SET(SOURCES
  main.cpp
  LibSL-small/src/LibSL/Math/Math.cpp
  LibSL-small/src/LibSL/Math/Vertex.cpp
  LibSL-small/src/LibSL/System/System.cpp
  LibSL-small/src/LibSL/CppHelpers/CppHelpers.cpp
)

add_executable(VoxModSynth ${SOURCES})

if(WIN32)
target_link_libraries(VoxModSynth shlwapi)
endif(WIN32)
