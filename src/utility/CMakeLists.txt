add_library(date_time 
    STATIC 
        date_time.cpp
        ${PROJECT_SOURCE_DIR}/include/utility/date_time.hpp
)

add_library(process_timer
    STATIC 
        process_timer.cpp
        ${PROJECT_SOURCE_DIR}/include/utility/process_timer.hpp
)

add_library(shared_memory 
    STATIC 
        shared_memory.cpp
        ${PROJECT_SOURCE_DIR}/include/utility/shared_memory.hpp
)

target_include_directories(date_time     PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(process_timer PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(shared_memory PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(pythonian)