cmake_minimum_required(VERSION 3.0)
project(DT_Flow)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)

message(STATUS DATATREE_HOME=${DATATREE_HOME})
list(APPEND CMAKE_PREFIX_PATH ${DATATREE_HOME})

link_directories(${DATATREE_HOME})
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
add_executable(correct Correct.cpp CorrectionTask.cpp Selector.cpp)
target_compile_definitions(correct PUBLIC _VANILLA_ROOT_)
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(correct QnCorrections Base Correction ${ROOT_LIBRARIES})