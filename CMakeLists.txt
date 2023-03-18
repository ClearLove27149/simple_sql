cmake_minimum_required(VERSION 3.24)
project(simple_sql)

set(CMAKE_CXX_STANDARD 11)

include_directories(btree/)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(simple_sql main.cpp
        db.cpp
        db.h
        table.cpp
        table.h
        cursor.cpp
        cursor.h
        index.cpp
        index.h
        util.cpp
        util.h)
