add_library(data_frame.cpp
    STATIC 
    data_frame.cpp
    ${PROJECT_SOURCE_DIR}/include/utility/pythonian/data_frame.hpp
)

target_include_directories(data_frame   PUBLIC ${PROJECT_SOURCE_DIR}/include)