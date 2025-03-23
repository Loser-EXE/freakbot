cmake_minimum_required(VERSION 3.31)

project(freakbot VERSION 0.1.0 DESCRIPTION "A bot made for A Kid Named Server :)")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

find_package(DPP REQUIRED)

target_link_libraries(${PROJECT_NAME} 
        ${DPP_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${DPP_INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)
