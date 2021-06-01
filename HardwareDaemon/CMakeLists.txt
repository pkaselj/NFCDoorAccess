project("HardwareDaemon")

include_directories("include")

add_executable(HardwareDaemon "src/Hardwared.cpp")

target_include_directories(HardwareDaemon PUBLIC "${Settings_SOURCE_DIR}/include"
												 "${Keypad_SOURCE_DIR}/include"
												 "${ErrorCodes_SOURCE_DIR}/include"
												 "${FIFO_Pipe_SOURCE_DIR}/include"
												 "${Mailbox_SOURCE_DIR}/include"
												 "${Logger_SOURCE_DIR}/include"
												 "${PN532_NFC_Driver_SOURCE_DIR}/include"
												 "${IndicatorController_SOURCE_DIR}/include")

target_link_libraries(HardwareDaemon InputControllerLib WatchdogClientLib KeypadLib ErrorCodesLib FIFO_PipeLib
	SimplifiedMailboxLib PN532_NFC_Lib DataMailboxLib IndicatorControllerLib pthread)



add_library(InputControllerLib SHARED "include/InputController.hpp" "src/InputController.cpp")
target_include_directories(InputControllerLib PUBLIC "${Mailbox_SOURCE_DIR}/include"
													  "${FIFO_Pipe_SOURCE_DIR}/include"
													  "${Time_SOURCE_DIR}/include"
													  "${IndicatorController_SOURCE_DIR}/include")
target_link_libraries(InputControllerLib InputAutomatonLib IndicatorControllerLib DataMailboxLib TimeLib FIFO_PipeLib)



add_library(InputAutomatonLib SHARED "include/InputAutomaton.hpp" "src/InputAutomaton.cpp")
target_include_directories(InputAutomatonLib PUBLIC "${Mailbox_SOURCE_DIR}/include"
													 "${MealyAutomaton_SOURCE_DIR}/include"
													 "${IndicatorController_SOURCE_DIR}/include")
target_link_libraries(InputAutomatonLib IndicatorControllerLib DataMailboxLib MealyAutomatonLib)
