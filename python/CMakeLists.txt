if(NOT DEFINED PROJECT_NAME)
  cmake_minimum_required(VERSION 3.7.2)

  # Project name and version
  project(cameopy VERSION 1.0.0 LANGUAGES CXX)
endif()

project(openPMDraytracepy VERSION 0.1.0 LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS True)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  # specify the C++ standard
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
endif()

#------------------------------------------------------------
# Dependencies
#------------------------------------------------------------
message(STATUS ${CMAKE_PREFIX_PATH})
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(openPMDraytrace REQUIRED) # provided by the C++ API
find_package(openPMD REQUIRED)

find_package(pybind11 CONFIG)
find_package(pybind11 2.4.3 REQUIRED)

pybind11_add_module(raytracingpy src/openPMD_io.cpp)

#file(GENERATE OUTPUT /tmp/message.log CONTENT $<TARGET_PROPERTY:cameo,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(raytracingpy PRIVATE raytrace)
#target_include_directories(raytracingpy
#  PRIVATE $<TARGET_PROPERTY:raytrace,INTERFACE_INCLUDE_DIRECTORIES>
#  )

#target_include_directories(${MODULE_NAME} ${cameo_INCLUDE_DIR})
set_target_properties(raytracingpy PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
  )


#---------------
# Library version
write_basic_package_version_file(raytracingpyConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  VERSION ${PROJECT_VERSION}
  )

if(INSTALL)
#------------------------------------------------------------
# Install
#------------------------------------------------------------
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cameopyConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# this is to install the headers
install(FILES ${CAMEO_PUBLIC_HEADER_FILES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  )

# in CMake 3.14 you just need #install(TARGETS cameo)
install(TARGETS cameopy
  EXPORT cameopyexport
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # windows
    COMPONENT cameopy_Runtime
    #LIBRARY DESTINATION  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/>
    LIBRARY DESTINATION  ${CMAKE_INSTALL_LIBDIR} # dynamic
    COMPONENT cameopy_Runtime
#    NAMELINK_COMPONENT cameo_Development #from CMake 3.12
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static
    COMPONENT cameopy_Development
  PUBLIC_HEADER  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )


# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT cameopyexport
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  FILE ${PROJECT_NAME}Config.cmake 
  )
# makes the project importable from the build directory
export(TARGETS cameopy FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake)


endif()
