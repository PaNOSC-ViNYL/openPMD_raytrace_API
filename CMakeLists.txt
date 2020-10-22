# git clone --depth 1 git@github.com:openPMD/openPMD-api.git
# cmake .. -DopenPMD_USE_PYTHON=OFF -DopenPMD_INSTALL=OFF -DopenPMD_USE_INTERNAL_CATCH=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_CLI_TOOLS=OFF
cmake_minimum_required(VERSION 3.7.2)

project(openPMD_raytrace_API LANGUAGES CXX)

include(GNUInstallDirs)


add_library(api  src/ray.hh src/rays.hh src/rays.cc)
set_target_properties(api
   PROPERTIES LINKER_LANGUAGE CXX
   )
# target_include_directories(component PUBLIC include/)
# set_property(TARGET component PROPERTY CXX_STANDARD 17)
# target_link_libraries(component PUBLIC openPMD::openPMD)

# add_library(wrap SHARED src/wrap.cc)
# target_include_directories(wrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(wrap PRIVATE component)
# set_property(TARGET wrap PROPERTY CXX_STANDARD 17)



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


