cmake_minimum_required(VERSION 3.7.2)

project(openPMDraytraceTest VERSION 0.1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(openPMDraytrace REQUIRED) # provided by the C++ API
find_package(openPMD REQUIRED)

#add_executable(test_read.exe bin/test_read.cpp)
#target_link_libraries(test_read.exe PRIVATE api)

add_executable(test_write.exe bin/test_write.cpp)
target_link_libraries(test_write.exe
  PRIVATE raytrace
  PRIVATE openPMD::openPMD)

# add_executable(test1.exe test1.cpp)
# target_link_libraries(test1.exe
#   PRIVATE raytrace
#   PRIVATE openPMD::openPMD)

#enable_testing()
#add_test(NAME write COMMAND  test_write.exe)
#add_test(NAME read COMMAND  test_read.exe)

#x  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/)

