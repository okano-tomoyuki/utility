add_library(date_time    
    STATIC
    date_time.cpp
)

target_include_directories(date_time
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

add_library(process_timer    
    STATIC
    process_timer.cpp
)

target_include_directories(process_timer
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

add_library(shared_memory    
    STATIC
    shared_memory.cpp
)

target_include_directories(shared_memory
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)