cmake_minimum_required(VERSION 3.20)
# Add vcpkg as dependency and package manager
# The additional dependencies will be installed
include(cmake/fetchvcpkg.cmake)
project(volume-rendering LANGUAGES CXX)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE sfml-system sfml-graphics sfml-window imgui::imgui ImGui-SFML::ImGui-SFML)
target_compile_features(main PRIVATE cxx_std_20)
set_target_properties(main PROPERTIES CXX_EXTENSIONS OFF)

if (MSVC)
    target_compile_options(main PRIVATE /W3)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()