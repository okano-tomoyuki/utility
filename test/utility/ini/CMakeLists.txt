if(${GLOBAL_USE_BUILD_LIBLARY})
    add_executable(test_ini
        test_ini.cpp 
        ${PROJECT_SOURCE_DIR}/include/utility/ini.hpp
    )
    target_link_libraries(test_ini ini)
else()
    add_executable(test_ini test_ini.cpp ${HEADERS})
endif()

target_include_directories(test_ini PUBLIC ${PROJECT_SOURCE_DIR}/include)