# git clone --depth 1 git@github.com:openPMD/openPMD-api.git
# cd openPMD-api
# patch -p1 < ../cmake_api.patch
# cmake .. -DopenPMD_USE_PYTHON=OFF -DopenPMD_INSTALL=OFF -DopenPMD_USE_INTERNAL_CATCH=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_CLI_TOOLS=OFF
cmake_minimum_required(VERSION 3.7.2)

project(openPMD_raytrace_API VERSION 0.1.0)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(ExternalProject)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS True)
endif()

##
option(TEST "Do testing" OFF)
option(EXTERNAL_PYBIND "Use Pybind v2.5.0 from GIT" OFF)
option(INSTALL "perform the installation" OFF)
#------------------------------------------------------------
if(NOT INSTALL)
  set(CMAKE_INSTALL_PREFIX /tmp/devel)
  set(pybind11_DIR /tmp/devel/)
endif()

#------------------------------------------------------------
find_package(pybind11 2.4.3)  # this is the version that has been used for development and tests
if(EXTERNAL_PYBIND OR NOT pybind11_FOUND)
  message(WARNING "Using PYBIND11 from GIT repository")
  ExternalProject_Add(pybind11 
	GIT_REPOSITORY https://github.com/pybind/pybind11.git
	GIT_TAG v2.5.0
	GIT_SHALLOW True
	INSTALL_COMMAND "make install"
	CMAKE_ARGS -DPYBIND11_TEST=OFF -DPYBIND11_INSTALL=ON -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
	)
  ExternalProject_Get_Property(pybind11 install_dir)
  set(pybind11_DIR ${install_dir}/src/pybind11-build/)
endif()


#########################################################
#------------------------------------------------------------
find_package(openPMD 0.12.1 QUIET)  # this is the version that has been used for development and tests
if(EXTERNAL_openPMD OR NOT openPMD_FOUND)
  message(WARNING "Using openPMD-api from GIT repository")
  ExternalProject_Add(openPMD-api
    GIT_REPOSITORY https://github.com/openPMD/openPMD-api.git
    GIT_TAG dev
    #GIT_COMMIT d6820a12b03b7c574a04e8c356a78b66492cb990
    GIT_SHALLOW True
    UPDATE_DISCONNECTED True
    PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/cmake_api.patch
#    INSTALL_COMMAND "make install"
    CMAKE_ARGS cmake .. -DopenPMD_USE_PYTHON=OFF -DopenPMD_INSTALL=ON -DopenPMD_USE_INTERNAL_CATCH=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_CLI_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    )
  ExternalProject_Get_Property(openPMD-api install_dir)
  set(openPMD_DIR ${install_dir}/src/openPMD-api-build/)
endif()


#------------------------------------------------------------
#find_package(openPMD_raytrace_API 0.1.0)
ExternalProject_Add(openPMDraytrace
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpp/
  DEPENDS openPMD-api
  #  DEPENDS pybind11
  CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DINSTALL=ON #-DopenPMD_DIR=${openPMD_DIR}
  #INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  #INSTALL_COMMAND ""
  #set(openPMDraytrace_DIR ${install_dir}/src/openPMDraytrace-build/)
  )


#------------------------------------------------------------
#set (CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
ExternalProject_Add(openPMDraytracepy
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python/
  DEPENDS openPMDraytrace
  #  DEPENDS pybind11
  CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DopenPMDraytrace_DIR=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake # -DopenPMD_DIR=${openPMD_pybind11_DIR} -Dcameo_DIR=${cameo_DIR}
  #INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  INSTALL_COMMAND ""
  )

################ Doc
# find_package(Doxygen)
# if (DOXYGEN_FOUND)
#   # set input and output files
#   set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#   set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  
#   # request to configure the file
#   configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#   message("Doxygen build started")
  
#   # note the option ALL which allows to build the docs together with the application
#   add_custom_target( doc 
#     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating API documentation with Doxygen"
#     VERBATIM )
# else (DOXYGEN_FOUND)
#   message("Doxygen need to be installed to generate the doxygen documentation")
# endif (DOXYGEN_FOUND)
