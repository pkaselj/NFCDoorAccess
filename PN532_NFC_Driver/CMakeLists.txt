project("PN532_NFC_Driver")

include_directories("include")

add_library(PN532_NFC_Lib SHARED "include/PN532_NFC.hpp" "src/PN532_NFC.cpp")
target_include_directories(PN532_NFC_Lib PUBLIC "${Logger_SOURCE_DIR}/include"
												"${Kernel_SOURCE_DIR}/include")
target_link_libraries(PN532_NFC_Lib nfc NulLoggerLib LoggerLib KernelLib)