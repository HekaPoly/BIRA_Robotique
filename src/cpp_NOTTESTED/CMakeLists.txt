//hmmm je vais devoir ajuster au fur et à mesure de la compilation
cmake_minimum_required(VERSION 3.8)
project(bira_motor_control)

set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)

include_directories(include)

add_library(uart_lib STATIC src/uart.cpp)

add_executable(motor_control_node
    src/control_moteurdenode.cpp
)

add_executable(uart_node
    src/uart_node.cpp
)

ament_target_dependencies(motor_control_node
    rclcpp
    std_msgs
    trajectory_msgs
)

ament_target_dependencies(uart_node
    rclcpp
    std_msgs
)

target_link_libraries(uart_node uart_lib)

install(TARGETS
    motor_control_node
    uart_node
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)

ament_package()