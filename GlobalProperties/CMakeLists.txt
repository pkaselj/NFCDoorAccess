project("GlobalProperties")

cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Xml REQUIRED)

include_directories("include")

configure_file("res/config.xml" "${Startup_BINARY_DIR}/config.xml" COPYONLY)

add_library(GlobalPropertiesLib SHARED "include/propertiesclass.h" "src/propertiesclass.cpp")
# qt5_use_modules(GlobalPropertiesLib Xml Core)
target_link_libraries(GlobalPropertiesLib Qt5::Core Qt5::Xml)