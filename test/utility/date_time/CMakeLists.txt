if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_date_time
        test_date_time.cpp 
        ${PROJECT_SOURCE_DIR}/include/utility/date_time.hpp
    )
    target_link_libraries(test_date_time date_time)
    target_include_directories(test_date_time PUBLIC ${PROJECT_SOURCE_DIR}/include)
else()
    add_executable(test_date_time test_date_time.cpp ${HEADERS})
endif()