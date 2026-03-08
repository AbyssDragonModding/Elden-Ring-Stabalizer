cmake_minimum_required(VERSION 3.25)
project("EldenRingStabalizer")

set(CMAKE_CXX_STANDARD 23) # C++ 2023
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_SOURCES
    ${CMAKE_SOURCE_DIR}/dllmain.cpp
    ${CMAKE_SOURCE_DIR}/pch.cpp
)
set(PROJ_HEADERS
    ${CMAKE_SOURCE_DIR}/pch.h
    ${CMAKE_SOURCE_DIR}/framework.h
)

# Add DLL
add_library(${PROJECT_NAME} SHARED
    ${PROJ_SOURCES}
    ${PROJ_HEADERS}
)
