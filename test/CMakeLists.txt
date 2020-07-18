cmake_minimum_required(VERSION 3.5)
project(test_resampling_sdreader C CXX)
set(CMAKE_CXX_STANDARD 11)

include_directories(../teensy)
include_directories(../src)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(test_suite1 test_no_loop_forward_playback.cpp ResamplingReaderFixture.h test_no_loop_forward_double_playbackrate.cpp)
target_link_libraries(test_suite1 teensy_cores_x86)
target_link_libraries(test_suite1 teensy-resampling-src)
target_link_libraries(test_suite1 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
