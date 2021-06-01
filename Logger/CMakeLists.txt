cmake_minimum_required (VERSION 3.8)

project ("Logger")

# include direcories
include_directories("include")

# shared libraries
add_library(LoggerLib SHARED "include/ILogger.hpp" "include/Logger.hpp" "src/Logger.cpp")
target_include_directories(LoggerLib PUBLIC "${Time_SOURCE_DIR}/include")
target_link_libraries(LoggerLib TimeLib)

add_library(NulLoggerLib SHARED "include/ILogger.hpp" "include/NulLogger.hpp" "src/NulLogger.cpp")

add_library(ThreadLoggerClientLib SHARED "include/ILogger.hpp" "include/ThreadLoggerClient.hpp" "src/ThreadLoggerClient.cpp")
target_include_directories(ThreadLoggerClientLib PUBLIC "${MailboxAPI_SOURCE_DIR}/include")
target_link_libraries(ThreadLoggerClientLib MailboxLib)

add_library(ThreadLoggerServerLib SHARED "include/ILogger.hpp" "include/ThreadLoggerServer.hpp" "src/ThreadLoggerServer.cpp")
target_link_libraries(ThreadLoggerServerLib NulLoggerLib ThreadLoggerClientLib)

add_library(LoggerToStdoutLib STATIC "include/ILogger.hpp" "include/LoggerToStdout.hpp")
target_include_directories(LoggerToStdoutLib PUBLIC "${Time_SOURCE_DIR}/include")
target_link_libraries(LoggerToStdoutLib TimeLib)
set_target_properties(LoggerToStdoutLib PROPERTIES LINKER_LANGUAGE CXX)




