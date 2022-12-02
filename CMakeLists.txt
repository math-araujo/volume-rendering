cmake_minimum_required(VERSION 3.20)
# Add vcpkg as dependency and package manager
# The additional dependencies will be installed
include(cmake/fetchvcpkg.cmake)
project(volume-rendering LANGUAGES CXX)

find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(OpenMP)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(unofficial-noise CONFIG REQUIRED)
find_package(unofficial-noiseutils CONFIG REQUIRED)

add_subdirectory(src)
function(set_executable executable main_file)
    add_executable(${executable} ${main_file} ${sources})
    target_link_libraries(${executable} PRIVATE volrender)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(${executable} PRIVATE OpenMP::OpenMP_CXX)
    else()
        message(WARNING "OpenMP not found")
    endif()

    target_compile_features(${executable} PRIVATE cxx_std_20)
    set_target_properties(${executable} PROPERTIES CXX_EXTENSIONS OFF)

    if (MSVC)
        target_compile_options(${executable} PRIVATE /W3)
    else()
        target_compile_options(${executable} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

set_executable("main" "src/main.cpp")
set_executable("fluid" "src/fluid.cpp")