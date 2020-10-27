# git clone --depth 1 git@github.com:openPMD/openPMD-api.git
# cd openPMD-api
# patch -p1 < ../cmake_api.patch
# cmake .. -DopenPMD_USE_PYTHON=OFF -DopenPMD_INSTALL=OFF -DopenPMD_USE_INTERNAL_CATCH=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_CLI_TOOLS=OFF
cmake_minimum_required(VERSION 3.7.2)

project(openPMD_raytrace_API VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS False)
endif()

##
option(TEST "Do testing" OFF)

#########################################################
add_subdirectory("openPMD-api")


add_library(api  src/openPMD_io.cc src/rays.cc)
set_target_properties(api
   PROPERTIES LINKER_LANGUAGE CXX
   )
target_include_directories(api
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ # all the private headers
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
  PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
set_target_properties(api PROPERTIES
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION ${PROJECT_VERSION}
  )

set_property(TARGET api PROPERTY CXX_STANDARD 11) # with 17 it crashes!
# it should be due to the openPMD API
target_link_libraries(api PUBLIC openPMD::openPMD)
if(ciao)


#---------------
# Library version
write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
  VERSION ${PROJECT_VERSION}
  )
endif()

if(INSTALL)
#------------------------------------------------------------
# Install
#------------------------------------------------------------
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# this is to install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ # don't forget the trailing /
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  )

# in CMake 3.14 you just need #install(TARGETS cameo)
install(TARGETS api
  EXPORT apiexport
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # windows
    COMPONENT api_Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # dynamic
    COMPONENT api_Runtime
#    NAMELINK_COMPONENT cameo_Development #from CMake 3.12
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # static
    COMPONENT api_Development
  PUBLIC_HEADER  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )


# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT apiexport
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  FILE ${PROJECT_NAME}Config.cmake 
  )
# makes the project importable from the build directory
export(TARGETS api FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
endif(INSTALL)

enable_testing()
add_subdirectory(test)

################ Doc
find_package(Doxygen)
if (DOXYGEN_FOUND)
  # set input and output files
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  
  # request to configure the file
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message("Doxygen build started")
  
  # note the option ALL which allows to build the docs together with the application
  add_custom_target( doc 
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)


