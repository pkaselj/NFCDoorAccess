cmake_minimum_required (VERSION 3.8)

project ("ProcessManager")

# include direcories
include_directories(ProcessManager "include")

# shared libraries
add_library(ProcessLib SHARED "include/Process.hpp" "src/Process.cpp")
add_library(ProcessManagerLib SHARED "include/ProcessManager.hpp" "src/ProcessManager.cpp")

target_include_directories(ProcessManagerLib PUBLIC "${Logger_SOURCE_DIR}/include"
													"${Kernel_SOURCE_DIR}/include")

target_link_libraries(ProcessManagerLib LoggerLib NulLoggerLib ProcessLib KernelLib)
