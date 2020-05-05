cmake_minimum_required(VERSION 3.0)
project(DT_Flow VERSION 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_PREFIX_PATH $ENV{DATATREE_HOME})
find_package(DataTree REQUIRED)

link_directories(${DataTree_LIBRARY_DIR})
add_executable(correct Correct.cpp Processing/task_factory.cc Processing/correction_task.cpp Processing/data_tree_var_manager.cc Processing/selector.cpp Processing/centrality.cc Processing/occupancy_corrections.cc Processing/efficiency.cc Processing/correction_task_fw3s.cc Processing/correction_task_fw3s.h Processing/correction_task_rnd.cc Processing/correction_task_rnd.h Processing/correction_task_fw3x.cc Processing/correction_task_fw3x.h Processing/correction_task_full.cc Processing/correction_task_full.h Processing/correlation_task_fw3s.cc Processing/correlation_task_fw3s.h)
target_compile_definitions(correct PUBLIC _VANILLA_ROOT_)
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(correct QnCorrections Base Correction Correlation DataTree ${ROOT_LIBRARIES})

add_executable(optimization Processing/correlation_task.cc optimize.cc Processing/correction_task.cpp Processing/data_tree_var_manager.cc Processing/selector.cpp Processing/centrality.cc Processing/occupancy_corrections.cc Processing/efficiency.cc Processing/efficiency_builder.cc)
target_compile_definitions(optimization PUBLIC _VANILLA_ROOT_)
target_include_directories(optimization PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(optimization QnCorrections Base Correction Correlation DataTree ${ROOT_LIBRARIES})

add_executable(correlate Processing/task_factory.cc Correlate.cpp Processing/correlation_task.cc Processing/correlation_task_fw3s.cc Processing/correlation_task_fw3s.h)
#target_compile_definitions(correlate PUBLIC "-DUSE_ROOT")
target_include_directories(correlate PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(correlate QnCorrections Base Correlation ${ROOT_LIBRARIES})

add_executable(occupancy occupancy.cc Processing/track_density.cc Processing/selector.cpp Processing/centrality.cc)
target_include_directories(occupancy PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(occupancy QnCorrections Base Correlation DataTree ${ROOT_LIBRARIES})

add_executable(efficiency CalculateEfficiency.cc Processing/efficiency_builder.cc)
target_include_directories(efficiency PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(efficiency ${ROOT_LIBRARIES})

add_executable(average Processing/averager.cc BuildAverage.cc Processing/efficiency.h)
target_include_directories(average PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(average ${ROOT_LIBRARIES})

add_executable(optimize optimization.cc Processing/efficiency_builder.h Processing/efficiency.h Processing/averager.h)
target_include_directories(optimize PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(optimize ${ROOT_LIBRARIES})