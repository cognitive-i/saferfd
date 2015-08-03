project(saferfd)
cmake_minimum_required(VERSION 2.8)
aux_source_directory("source" SRC_LIST)
aux_source_directory("example" EG_LIST)


if(CMAKE_COMPILER_IS_GNUCXX)
  list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST} ${EG_LIST})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

################################################################################
# unit tests
ADD_SUBDIRECTORY(gmock-1.7.0)
enable_testing()

aux_source_directory("test" TEST_SRC_LIST)
list(APPEND TEST_SRC_LIST ${SRC_LIST})

include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/gtest/include source)
add_executable(testSaferFd ${TEST_SRC_LIST})

target_link_libraries(testSaferFd gtest gtest_main gmock)
set_property(TARGET testSaferFd PROPERTY CXX_STANDARD 11)
set_property(TARGET testSaferFd PROPERTY CXX_STANDARD_REQUIRED ON)
add_test(testSaferFd testSaferFd)
