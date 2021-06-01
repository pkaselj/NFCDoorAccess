# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(COMPILE_TESTS											ON)
set(RUNTIME_BUFFER_OVERFLOW_DETECTION						OFF)
set(RUNTIME_BOUNDS_CHECKING_FOR_CXX_STRINGS_AND_CONTAINERS	OFF)
set(UNWIND_TABLES											OFF)
set(WALL_FLAG											    OFF)
set(WPEDANTIC_FLAG											OFF)
set(WEXTRA_FLAG												OFF)

IF(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(RUNTIME_BUFFER_OVERFLOW_DETECTION)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_FORTIFY_SOURCE=2")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_FORTIFY_SOURCE=2")
ENDIF(RUNTIME_BUFFER_OVERFLOW_DETECTION)

IF(RUNTIME_BOUNDS_CHECKING_FOR_CXX_STRINGS_AND_CONTAINERS)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_ASSERTIONS")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_ASSERTIONS")
ENDIF(RUNTIME_BOUNDS_CHECKING_FOR_CXX_STRINGS_AND_CONTAINERS)

IF(UNWIND_TABLES)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fasynchronous-unwind-tables")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fasynchronous-unwind-tables")
ENDIF(UNWIND_TABLES)

IF(WALL_FLAG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
ENDIF(WALL_FLAG)

IF(WPEDANTIC_FLAG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wpedantic")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wpedantic")
ENDIF(WPEDANTIC_FLAG)

IF(WEXTRA_FLAG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra")
ENDIF(WEXTRA_FLAG)



#set(GCC_COVERAGE_COMPILE_FLAGS "-ggdb3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

project ("NFCDoorAccess")

add_subdirectory ("Watchdog")
add_subdirectory ("Startup")
add_subdirectory ("Settings")
add_subdirectory ("ProcessManager")
add_subdirectory ("Time")
add_subdirectory ("Logger")
add_subdirectory ("Kernel")
add_subdirectory ("UNIX_SignalHandler")
add_subdirectory ("SQLite3_Linux")
add_subdirectory ("SQLite3_Database")
add_subdirectory ("DatabaseGateway")
add_subdirectory ("MealyAutomaton")
add_subdirectory ("MailboxAutomaton")
add_subdirectory ("MailboxAPI")
add_subdirectory ("ErrorCodes")
add_subdirectory ("FIFO_Pipe")
add_subdirectory ("GPIO")
add_subdirectory ("HardwareDaemon")
add_subdirectory ("Keypad")
add_subdirectory ("RPi_RFID")
add_subdirectory ("RFIDController")
add_subdirectory ("SharedMemory")
add_subdirectory ("MainApplication")
add_subdirectory ("Mailbox")
add_subdirectory ("DatabaseTables")
add_subdirectory ("PN532_NFC_Driver")
add_subdirectory ("IndicatorController")
add_subdirectory ("GlobalProperties")
# add_subdirectory ("WebAPI")



if(COMPILE_TESTS)
	add_subdirectory ("TestPrograms")
endif()