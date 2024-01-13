if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_data_frame 
        test_data_frame.cpp 
        ${PROJECT_SOURCE_DIR}/include/utility/pythonian/data_frame.hpp
    )
    target_link_libraries(test_data_frame data_frame)
    target_include_directories(test_data_frame PUBLIC ${PROJECT_SOURCE_DIR}/include)
else()
    add_executable(test_data_frame test_data_frame.cpp ${HEADERS})
endif()