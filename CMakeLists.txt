cmake_minimum_required(VERSION 2.8)

project(ALF)

include_directories( "${WIND_BASE}/target/h/WPILib" )

set(SRC
    Main.cpp
    Commands.cpp
)

add_executable(ALF.out ${SRC})
