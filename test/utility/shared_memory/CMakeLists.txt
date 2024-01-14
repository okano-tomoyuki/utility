if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_shared_memory_1 
        test_shared_memory_1.cpp
        sample_data.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/shared_memory.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/date_time.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/process_timer.hpp
    )
    add_executable(test_shared_memory_2 
        test_shared_memory_2.cpp 
        sample_data.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/shared_memory.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/date_time.hpp 
        ${PROJECT_SOURCE_DIR}/include/utility/process_timer.hpp
    )
    target_link_libraries(test_shared_memory_1
        shared_memory 
        date_time 
        process_timer
    )
    target_link_libraries(test_shared_memory_2 
        shared_memory 
        date_time 
        process_timer
    )
else()
    add_executable(test_shared_memory_1 test_shared_memory_1.cpp ${HEADERS})
    add_executable(test_shared_memory_2 test_shared_memory_2.cpp ${HEADERS})
endif()

target_include_directories(test_shared_memory_1 PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(test_shared_memory_2 PUBLIC ${PROJECT_SOURCE_DIR}/include)