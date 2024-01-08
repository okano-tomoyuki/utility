add_executable(date_time.out 
    date_time.cpp
)

target_link_libraries(date_time.out 
    date_time
)

add_executable(process_timer.out 
    process_timer.cpp
)

target_link_libraries(process_timer.out 
    process_timer
    date_time
)

add_executable(shared_memory.out 
    shared_memory.cpp
)

target_link_libraries(shared_memory.out 
    shared_memory
)
