

#add_executable(test_read.exe bin/test_read.cpp)
#target_link_libraries(test_read.exe PRIVATE api)

add_executable(test_write.exe bin/test_write.cpp)
target_link_libraries(test_write.exe PRIVATE api
  PRIVATE openPMD::openPMD)

add_executable(test1.exe test1.cpp)
target_link_libraries(test1.exe
  PRIVATE api
  PRIVATE openPMD::openPMD)


add_test(NAME write COMMAND  test_write.exe)
#add_test(NAME read COMMAND  test_read.exe)

#x  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/)

