cmake_minimum_required(VERSION 3.21)
project(Lab5_Daniela LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(Lab5_Daniela main.cpp)
target_link_libraries(Lab5_Daniela PRIVATE SFML::Graphics SFML::Window SFML::System)

add_executable(Lab5_Daniela_Experiments Experiments.cpp)
add_executable(Lab5_Daniela_Tests ModelTests.cpp)

enable_testing()
add_test(NAME Lab5_Daniela_PhysicsModel COMMAND Lab5_Daniela_Tests)
