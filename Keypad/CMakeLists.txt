project("Keypad")

include_directories("include")

add_library(KeypadLib SHARED "include/keypad.hpp" "src/keypad.cpp")

target_include_directories(KeypadLib PUBLIC "${FIFO_Pipe_SOURCE_DIR}/include"
											"${Logger_SOURCE_DIR}/include"
											"${GlobalProperties_SOURCE_DIR}/include")

target_link_libraries(KeypadLib FIFO_PipeLib NulLoggerLib GlobalPropertiesLib)