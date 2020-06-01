cmake_minimum_required(VERSION 3.16)
project(AVC_robot)

set(CMAKE_CXX_STANDARD 14)


set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5.1 COMPONENTS graphics audio network window system REQUIRED)
add_executable(AVC_robot robot.cpp)
target_link_libraries(AVC_robot sfml-graphics sfml-audio sfml-network sfml-window sfml-system)
