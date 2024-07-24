cmake_minimum_required(VERSION 3.10)

project(ParSTLTests)

find_package(TBB REQUIRED)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set(CMAKE_CXX_COMPILER icpx)
add_compile_options(-fsycl -fsycl-pstl-offload=gpu)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        TBB::tbb
)
