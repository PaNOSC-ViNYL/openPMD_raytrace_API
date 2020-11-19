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
# add_custom_command(TARGET raytracing.py POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_CURRENT_SOURCE_DIR}/python/
#         ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing
#     )
set_target_properties(raytracingpy PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
  )

#####------------------------------------------- copied from CMakeLists.txt of openPMD-api
# ancient Clang releases: 
#   https://github.com/openPMD/openPMD-api/issues/542
#   https://pybind11.readthedocs.io/en/stable/faq.html#recursive-template-instantiation-exceeded-maximum-depth-of-256
#   https://bugs.llvm.org/show_bug.cgi?id=18417
#   https://github.com/llvm/llvm-project/commit/e55b4737c026ea2e0b44829e4115d208577a67b2
#if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" AND
#      CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1) OR
#    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND
#      CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.0))
#  message(STATUS "Clang: Passing -ftemplate-depth=1024")
#  target_compile_options(openPMD.py
#    PRIVATE -ftemplate-depth=1024)
#endif()))

if(WIN32)
  set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
    "${CMAKE_INSTALL_LIBDIR}/site-packages"
    )
else()
  set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
    "${CMAKE_INSTALL_LIBDIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages"
    )
endif()
set(CMAKE_INSTALL_PYTHONDIR "${CMAKE_INSTALL_PYTHONDIR_DEFAULT}"
  CACHE STRING "Location for installed python package"
  )
set(CMAKE_PYTHON_OUTPUT_DIRECTORY
  "${openPMD_BINARY_DIR}/${CMAKE_INSTALL_PYTHONDIR}"
  CACHE PATH "Build directory for python modules"
  )
# set_target_properties(raytracing.py PROPERTIES
# #  ARCHIVE_OUTPUT_NAME openpmd_api_cxx
# #  LIBRARY_OUTPUT_NAME openpmd_api_cxx
#   ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api_api
#   COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   )
# add_custom_command(TARGET raytracing.py POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory
#   ${openPMD_SOURCE_DIR}/python/src/raytracing_api
#   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/raytracing_api
#   )

##### End of ------------------------------------ copied from CMakeLists.txt of openPMD-api
#---------------
# Library version
write_basic_package_version_file(raytracingpyConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  VERSION ${PROJECT_VERSION}
  )


#------------------------------------------------------------
# Install
#------------------------------------------------------------
install(
  FILES   ${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py
  DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/raytracing
  )
install(TARGETS raytracingpy
  DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/raytracing
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/raytracingpyConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

if(INSTALL)
#############################################

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
