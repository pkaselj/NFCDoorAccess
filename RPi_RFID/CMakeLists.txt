project("RPi_RFID")

include_directories("include")

add_library(RPiRFIDLib SHARED "include/MFRC522.h" "src/MFRC522.cpp")

target_link_libraries(RPiRFIDLib bcm2835)