project("WebAPI")

include_directories("include")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Network REQUIRED)

message("=========================== ${QT_INCLUDES}")

add_executable(WebAPIController "src/WebAPIController.cpp")
qt5_use_modules(WebAPIController Network)
# target_include_directories(WebAPIController PUBLIC "${QT_INCLUDES}")
# target_link_libraries(WebAPIController Qt5::Core Qt5::Network)