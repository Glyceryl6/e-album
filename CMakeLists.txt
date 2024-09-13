cmake_minimum_required(VERSION 3.23)
project(e_album C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(e_album
        lcd.c
        lcd.h
        list.h
        main.c)
