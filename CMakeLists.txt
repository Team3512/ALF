cmake_minimum_required(VERSION 2.8)
include(FRCDeploy)

project(ALF)
find_package(WPILib)
include_directories(${WPILib_INCLUDE_DIRS})

set(DIRS)

file(GLOB SRC *.cpp *.c)

foreach(ELEM ${DIRS})
    file(GLOB_RECURSE FILES ${ELEM}.cpp ${ELEM}.c)
    list(APPEND SRC ${FILES})
endforeach(ELEM)

add_executable(ALF.out ${SRC})

target_link_libraries(ALF.out ${WPILib_LIBRARY})

add_make_deploy(ALF.out 10.35.12.2)
