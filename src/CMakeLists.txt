cmake_minimum_required(VERSION 3.0)
project(DT_Flow VERSION 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_PREFIX_PATH $ENV{DATATREE_HOME})
find_package(DataTree REQUIRED)

link_directories(${DataTree_LIBRARY_DIR})
add_executable(correct Correct.cpp CorrectionTask.cpp DataTreeVarManager.cpp Selector.cpp Centrality.cxx occupancy_corrections.cc efficiency.cc)
target_compile_definitions(correct PUBLIC _VANILLA_ROOT_)
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(correct QnCorrections Base Correction DataTree ${ROOT_LIBRARIES})

add_executable(correlate Correlate.cpp CorrelationTask.cpp)
#target_compile_definitions(correlate PUBLIC "-DUSE_ROOT")
target_include_directories(correlate PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(correlate QnCorrections Base Correlation ${ROOT_LIBRARIES})

add_executable(occupancy occupancy.cc TrackDensity.cc Selector.cpp Centrality.cxx)
target_include_directories(occupancy PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(occupancy QnCorrections Base Correlation DataTree ${ROOT_LIBRARIES})

add_executable(efficiency CalculateEfficiency.cc efficiency_builder.cc)
target_include_directories(efficiency PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(efficiency ${ROOT_LIBRARIES})

add_executable(average averager.h BuildAverage.cc efficiency.h)
target_include_directories(average PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(average ${ROOT_LIBRARIES})

add_executable(optimize optimization.cc efficiency_builder.h efficiency.h averager.h)
target_include_directories(optimize PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(optimize ${ROOT_LIBRARIES})