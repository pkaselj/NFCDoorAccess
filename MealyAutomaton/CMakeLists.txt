project("MealyAutomaton")

include_directories("include")

add_library(MealyAutomatonLib SHARED "include/mautomat.h" "include/mautomatfunctor.h" "src/mautomat.cpp")

target_include_directories(MealyAutomatonLib PUBLIC "${Time_SOURCE_DIR}/include"
													"${Kernel_SOURCE_DIR}/include")

target_link_libraries(MealyAutomatonLib TimeLib KernelLib NulLoggerLib)