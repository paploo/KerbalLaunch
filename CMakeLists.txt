cmake_minimum_required(VERSION 3.15)
project(KerbalLaunch C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(KerbalLaunch
        frame.c
        frame.h
        main.c
        optimizer.c
        optimizer.h
        planetoid.c
        planetoid.h
        program.c
        program.h
        rocket.c
        rocket.h
        statistics.c
        statistics.h
        system.c
        system.h
        vector.c
        vector.h)
