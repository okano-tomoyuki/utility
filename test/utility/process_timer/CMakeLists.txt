if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_process_timer 
        test_process_timer.cpp 
        ${PROJECT_SOURCE_DIR}/include/utility/date_time.hpp
        ${PROJECT_SOURCE_DIR}/include/utility/process_timer.hpp
    )
    target_link_libraries(test_process_timer 
        process_timer 
        date_time
    )
else()
    add_executable(test_process_timer test_process_timer.cpp ${HEADERS})
endif()

target_include_directories(test_process_timer PUBLIC ${PROJECT_SOURCE_DIR}/include)