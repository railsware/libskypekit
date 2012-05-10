cmake_minimum_required(VERSION 2.6)

if($ENV{DEBUG})
  message("** Enable debug")
  set(CMAKE_BUILD_TYPE Debug)
endif($ENV{DEBUG})

set(SKYPEKIT_SDK $ENV{SKYPEKIT_SDK})

if(SKYPEKIT_SDK)
  message("** Using SkypeKit SDK: ${SKYPEKIT_SDK}")
else(SKYPEKIT_SDK)
  message("** Please specify path to SkypeKit SDK using SKYPEKIT_SDK environment")
  return(1)
endif(SKYPEKIT_SDK)

set(CMAKE_CXX_FLAGS " -Wall -O2 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb -pthread -DDEBUG")

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

find_library(SKYPEKIT_SDK_CPPWRAPPER_LIB
  NAMES skypekit-cppwrapper_2_lib
  PATHS ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/build)

find_library(SKYPEKIT_SDK_CYASSL_LIB
  NAMES skypekit-cyassl_lib
  PATHS ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/build)

include_directories(
  ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/src/api
  ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/src/ipc
  ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/src/types
  ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/src/platform/threading
  ${SKYPEKIT_SDK}/interfaces/skype/cpp_embedded/src/platform/threading/pthread
  ${SKYPEKIT_SDK}/ipc/cpp
  ${SKYPEKIT_SDK}/ipc/cpp/platform/se
  ${SKYPEKIT_SDK}/ipc/cpp/ssl/cyassl/include
  ${SKYPEKIT_SDK}/ipc/cpp/ssl/cyassl/ctaocrypt/include
  ${SKYPEKIT_SDK}/ipc/cpp/ssl/cyassl/include/openssl
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)

add_library(skypekit SHARED ${SRCS})

# Set the build version (VERSION) and the API version (SOVERSION)
set_target_properties(skypekit PROPERTIES VERSION 1.0.0 SOVERSION 1)

target_link_libraries(skypekit ${SKYPEKIT_SDK_CPPWRAPPER_LIB} ${SKYPEKIT_SDK_CYASSL_LIB})

install(TARGETS skypekit
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
