cmake_minimum_required(VERSION 3.5)
project(project_builder)

set(CMAKE_CXX_STANDARD 17)
set(SYS_HEADERS /opt/homebrew/Cellar/llvm/16.0.4/include/c++/v1)

include_directories(.)

set (SOURCE_FILES
        main.cpp
        converter.h
        builder.h
        mapper.h
        common_types.h
        shared.h
        project_exceptions.h)

add_executable(project_builder
               ${SOURCE_FILES})
