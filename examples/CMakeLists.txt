cmake_minimum_required(VERSION 2.6)

if($ENV{DEBUG})
  message("** Enable debug")
  set(CMAKE_BUILD_TYPE Debug)
endif($ENV{DEBUG})

set(CMAKE_C_FLAGS "-Wall -O2 -pthread")
set(CMAKE_C_FLAGS_DEBUG "-Wall -ggdb -pthread -DDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

find_library(SKYPEKIT_LIB
  NAMES skypekit
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(skypekit_ping_pong ${CMAKE_CURRENT_SOURCE_DIR}/ping_pong.c)

target_link_libraries(skypekit_ping_pong ${SKYPEKIT_LIB})

