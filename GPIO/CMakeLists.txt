project("GPIO")

include_directories("include")

add_library(GPIO_Lib SHARED "include/rpi_gpio.h" "src/rpi_gpio.c")
target_include_directories(GPIO_Lib PUBLIC "${ErrorCodes_SOURCE_DIR}/include")
target_link_libraries(GPIO_Lib ErrorCodesLib)