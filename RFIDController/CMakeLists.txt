project("RFIDController")

include_directories("include")

add_library(RFIDControllerLib SHARED "include/RFID_Controller.hpp" "src/RFID_Controller.cpp")

target_include_directories(RFIDControllerLib PUBLIC "${RPi_RFID_SOURCE_DIR}/include"
													"${Logger_SOURCE_DIR}/include"
													"${Kernel_SOURCE_DIR}/include"
													"${Time_SOURCE_DIR}/include")

target_link_libraries(RFIDControllerLib NulLoggerLib RPiRFIDLib KernelLib TimeLib)