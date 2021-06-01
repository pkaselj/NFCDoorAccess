project("MainApplication")

include_directories("include")

add_executable(MainApplication "src/MainApplication.cpp")

target_include_directories(MainApplication PUBLIC "${Settings_SOURCE_DIR}/include"
												  "${FIFO_Pipe_SOURCE_DIR}/include"
												  "${Mailbox_SOURCE_DIR}/include"
												  "${ErrorCodes_SOURCE_DIR}/include"
												  "${UNIX_SignalHandler_SOURCE_DIR}/include"
												  "${IndicatorController_SOURCE_DIR}/include")

target_link_libraries(MainApplication MainAutomatonLib KeypadAutomatonLib MainAutomatonLib WatchdogClientLib FIFO_PipeLib ErrorCodesLib IndicatorControllerLib)





add_library(MainAutomatonLib SHARED "include/MainAutomaton.hpp" "src/MainAutomaton.cpp")

target_include_directories(MainAutomatonLib PUBLIC "${MealyAutomaton_SOURCE_DIR}/include"
												   "${Mailbox_SOURCE_DIR}/include"
												   "${UNIX_SignalHandler_SOURCE_DIR}/include"
												   "${IndicatorController_SOURCE_DIR}/include")

target_link_libraries(MainAutomatonLib MealyAutomatonLib DataMailboxLib IndicatorControllerLib rt)





add_library(KeypadAutomatonLib SHARED "include/KeypadAutomaton.hpp" "src/KeypadAutomaton.cpp")

target_include_directories(KeypadAutomatonLib PUBLIC "${MealyAutomaton_SOURCE_DIR}/include"
													 "${Mailbox_SOURCE_DIR}/include"
													 "${UNIX_SignalHandler_SOURCE_DIR}/include"
													 "${Time_SOURCE_DIR}/include"
													 "${IndicatorController_SOURCE_DIR}/include")

target_link_libraries(KeypadAutomatonLib MealyAutomatonLib DataMailboxLib UNIX_SignalHandlerLib IndicatorControllerLib TimerLib rt)





add_library(AutomatonPairFactoryLib STATIC "include/AutomatonPairFactory.hpp")

set_target_properties(AutomatonPairFactoryLib PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(AutomatonPairFactoryLib PUBLIC "${MealyAutomaton_SOURCE_DIR}/include"
													 "${Mailbox_SOURCE_DIR}/include")

target_link_libraries(AutomatonPairFactoryLib MainAutomatonLib KeypadAutomatonLib MealyAutomatonLib DataMailboxLib)