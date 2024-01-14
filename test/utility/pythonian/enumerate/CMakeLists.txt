if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_enumerate 
        test_enumerate.cpp 
        ${PROJECT_SOURCE_DIR}/include/utility/pythonian/enumerate.hpp
    )
    target_link_libraries(test_enumerate enumerate)
else()
    add_executable(test_enumerate test_enumerate.cpp ${HEADERS})
endif()

target_include_directories(test_enumerate PUBLIC ${PROJECT_SOURCE_DIR}/include)