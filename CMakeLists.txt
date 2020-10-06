cmake_minimum_required(VERSION 3.17)
project(project_so_01 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-pthread -lm")

#add_executable(operational_system_group PP_03/statistics_mt.c)
#add_executable(operational_system_group PP_03/calcpi.c)
add_executable(operational_system_group PP_03/multmatriz.c)
#add_executable(operational_system_group source_tests/teste_matriz.c)
#target_link_libraries(operational_system_group m)