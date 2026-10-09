# DLL Version 10.4.1 vom 09.10.2026
cmake_minimum_required(VERSION 3.16)

project(HelperClasses LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

if(ANDROID)
    include(C:/Users/Reinhard/AppData/Local/Android/Sdk/android_openssl/android_openssl.cmake)
else()
    find_package(OpenSSL REQUIRED)
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

add_library(HelperClasses SHARED
  HelperClasses_global.h
  array.t bytearray.cpp bytearray.h
  basicrow.cpp basicrow.h basictable.cpp basictable.h
  secure.cpp secure.h
  securefile.cpp securefile.h
)

if(ANDROID)
    add_android_openssl_libraries(SecureAndroid)
else()
    target_link_libraries(HelperClasses PRIVATE ${OPENSSL_LIBRARIES})
endif()

target_link_libraries(HelperClasses PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(HelperClasses PRIVATE HELPERCLASSES_LIBRARY)
