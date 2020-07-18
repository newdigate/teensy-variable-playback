cmake_minimum_required(VERSION 3.5)
project(test_resampling_sdreader C CXX)
set(CMAKE_CXX_STANDARD 11)

include_directories(../teensy_cores_x86)
include_directories(../common)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(test_resampling_sdreader runtests.cpp)
target_link_libraries(test_resampling_sdreader teensy_cores_x86)
target_link_libraries(test_resampling_sdreader resampling_common)

add_executable(test_suite1 readertests.cpp ResamplingReaderFixture.h)
target_link_libraries(test_suite1 teensy_cores_x86)
target_link_libraries(test_suite1 resampling_common)
target_link_libraries(test_suite1 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

enable_testing()
#add_test(NAME test_suite1 COMMAND test_suite1)