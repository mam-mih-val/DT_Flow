cmake_minimum_required(VERSION 3.0)
project(DT_Flow VERSION 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_PREFIX_PATH $ENV{DATATREE_HOME})
find_package(DataTree REQUIRED)
link_directories(${DataTree_LIBRARY_DIR})

set(SOURCES
        Processing/data_tree_var_manager.cc
        Processing/selector.cc
        Processing/centrality.cc
        Processing/occupancy_corrections.cc
        Processing/correction_task.cc
        Processing/correction_task_fw3s.cc
        Processing/correction_task_fw3x.cc
        Processing/correction_task_rnd.cc
        Processing/correction_task_full.cc
        Processing/correlation_task.cc
        Processing/correlation_task_fw3s.cc
        Processing/correlation_task_fw3x.cc
        Processing/correlation_task_rnd.cc
        Processing/task_factory.cc
        Processing/task_factory_fw3s.cc Processing/task_factory_fw3s.h Processing/task_factory_rnd.cc Processing/task_factory_rnd.h Processing/task_factory_track_density.cc Processing/task_factory_track_density.h)
set(HEADERS
        Processing/data_tree_var_manager.h
        Processing/selector.h
        Processing/centrality.h
        Processing/occupancy_corrections.h
        Processing/correction_task.h
        Processing/correction_task_fw3s.h
        Processing/correction_task_fw3x.h
        Processing/correction_task_rnd.h
        Processing/correction_task_full.h
        Processing/correlation_task.h
        Processing/correlation_task_fw3s.h
        Processing/correlation_task_fw3x.h
        Processing/correlation_task_rnd.h
        Processing/task_factory.h)

add_executable(correct Correct.cpp Processing/task_factory.cc Processing/correction_task.cc Processing/data_tree_var_manager.cc Processing/selector.cc Processing/centrality.cc Processing/occupancy_corrections.cc Processing/efficiency.cc Processing/correction_task_fw3s.cc Processing/correction_task_fw3s.h Processing/correction_task_rnd.cc Processing/correction_task_rnd.h Processing/correction_task_fw3x.cc Processing/correction_task_fw3x.h Processing/correction_task_full.cc Processing/correction_task_full.h Processing/correlation_task_fw3s.cc Processing/correlation_task_fw3s.h Processing/correlation_task_rnd.cc Processing/correlation_task_rnd.h Processing/correlation_task_fw3x.cc Processing/correlation_task_fw3x.h run_tasks.cc)
target_compile_definitions(correct PUBLIC _VANILLA_ROOT_)
target_include_directories(correct PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(correct QnCorrections Base Correction Correlation DataTree ${ROOT_LIBRARIES})

add_executable(process run_tasks.cc ${SOURCES})
target_compile_definitions(process PUBLIC _VANILLA_ROOT_)
target_include_directories(process PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(process QnCorrections Base Correction Correlation DataTree ${ROOT_LIBRARIES})

add_executable(optimization Processing/correlation_task.cc optimize.cc Processing/correction_task.cc Processing/data_tree_var_manager.cc Processing/selector.cc Processing/centrality.cc Processing/occupancy_corrections.cc Processing/efficiency.cc Processing/efficiency_builder.cc)
target_compile_definitions(optimization PUBLIC _VANILLA_ROOT_)
target_include_directories(optimization PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${DATATREE_HOME})
target_link_libraries(optimization QnCorrections Base Correction Correlation DataTree ${ROOT_LIBRARIES})

add_executable(correlate Processing/task_factory.cc Correlate.cpp Processing/correlation_task.cc Processing/correlation_task_fw3s.cc Processing/correlation_task_fw3s.h Processing/correlation_task_rnd.cc Processing/correlation_task_rnd.h Processing/correlation_task_fw3x.cc Processing/correlation_task_fw3x.h)
#target_compile_definitions(correlate PUBLIC "-DUSE_ROOT")
target_include_directories(correlate PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(correlate QnCorrections Base Correlation ${ROOT_LIBRARIES})

add_executable(occupancy occupancy.cc Processing/track_density.cc Processing/selector.cc Processing/centrality.cc)
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