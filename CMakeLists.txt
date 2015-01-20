cmake_minimum_required (VERSION 2.6)
project(deltaspin)

# check for in-source build, forbid it!
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "\nIn-source build attempt detected!\n"
    "Please create a new directory (e.g. build) and run `cmake ..`.\n"
    "Also don't forget to delete the created CMakeCache.txt and CMakeFiles dir"
    )
endif()

# find CERN ROOT and Pluto and set it up
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(ROOT "5.30" REQUIRED)
find_package(Pluto)
include_directories(${ROOT_INCLUDES})
link_directories(${ROOT_LIBRARY_DIR})
include_directories(${PLUTO_INCLUDE_PATH})
link_directories(${PLUTO_LIBRARY_PATH})

# some more cmake setup 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# really no optimization in debug mode
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall")
endif()

# set default build type if unspecified so far
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# build the different executables
add_executable(data_analysis
        data_analysis.cpp
   )
target_link_libraries(data_analysis ${ROOT_LIBRARIES})

add_executable(data_analysis_master
        data_analysis_master.cpp
   )
target_link_libraries(data_analysis_master ${ROOT_LIBRARIES})

add_executable(pluto_analysis
    pluto_analysis.cpp
   )
target_link_libraries(pluto_analysis ${PLUTO_LIBRARY} ${ROOT_LIBRARIES})

add_executable(pluto_analysis_master
    pluto_analysis_master.cpp
   )
target_link_libraries(pluto_analysis_master ${PLUTO_LIBRARY} ${ROOT_LIBRARIES})
