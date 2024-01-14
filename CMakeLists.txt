cmake_minimum_required(VERSION 3.13)
project(Utility)

if(UNIX)
    set(CMAKE_C_COMPILER   g++)
    set(CMAKE_CXX_COMPILER g++)
else()
    set(CMAKE_C_COMPILER   bcc32c)
    set(CMAKE_CXX_COMPILER bcc32c)
endif()

if(${GLOBAL_USE_BUILD_LIBLARY})
    add_compile_definitions(GLOBAL_USE_BUILD_LIBLARY)
    add_subdirectory(src)
    add_subdirectory(test)
else()
    file(GLOB_RECURSE HEADERS
        ${PROJECT_SOURCE_DIR}/include/*.hpp 
        ${PROJECT_SOURCE_DIR}/include/*.h
    )
    add_subdirectory(test)
endif()