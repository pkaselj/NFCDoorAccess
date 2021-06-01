cmake_minimum_required (VERSION 3.8)

project("Kernel")

include_directories("include")

add_library(KernelLib SHARED "src/Kernel.cpp" "include/Kernel.hpp")

target_include_directories(KernelLib PUBLIC "${Logger_SOURCE_DIR}/include" 
				                            "${Settings_SOURCE_DIR}/include"
											"${GlobalProperties_SOURCE_DIR}/include")
target_link_libraries(KernelLib LoggerLib GlobalPropertiesLib)