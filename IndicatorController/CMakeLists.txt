project("IndicatorController")

include_directories("include")

add_library(IndicatorControllerLib SHARED "include/IndicatorController.hpp" "src/IndicatorController.cpp")
target_include_directories(IndicatorControllerLib PUBLIC "${Kernel_SOURCE_DIR}/include"
														 "${Mailbox_SOURCE_DIR}/include")
target_link_libraries(IndicatorControllerLib DataMailboxLib KernelLib LoggerLib BuzzerControllerLib DoorControllerLib I2C_LCD_ControllerLib pigpio)


add_library(BuzzerControllerLib SHARED "include/BuzzerController.hpp" "src/BuzzerController.cpp")
target_include_directories(BuzzerControllerLib PUBLIC "${Kernel_SOURCE_DIR}/include")
target_link_libraries(BuzzerControllerLib KernelLib LoggerLib pigpio)



add_library(DoorControllerLib SHARED "include/DoorController.hpp" "src/DoorController.cpp")
target_include_directories(DoorControllerLib PUBLIC "${Kernel_SOURCE_DIR}/include"
													"${Time_SOURCE_DIR}/include")
target_link_libraries(DoorControllerLib KernelLib LoggerLib TimerLib pigpio)



add_library(I2C_LCD_ControllerLib SHARED "include/I2C_LCD_Controller.hpp" "src/I2C_LCD_Controller.cpp")
target_include_directories(I2C_LCD_ControllerLib PUBLIC "${Kernel_SOURCE_DIR}/include"
														"${Time_SOURCE_DIR}/include")
target_link_libraries(I2C_LCD_ControllerLib KernelLib LoggerLib TimerLib NulLoggerLib pigpio)