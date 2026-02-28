cmake_minimum_required(VERSION 3.10)
project(bira_motion_planning)

# Standard C++ 17
set(CMAKE_CXX_STANDARD 17)

# 1. Dépendances
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_core REQUIRED)
find_package(moveit_ros_planning REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# 2. Génération du Service
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/PlanTrajectory.srv"
  DEPENDENCIES sensor_msgs geometry_msgs moveit_msgs
)

# 3. Compilation du Serveur
add_executable(motion_planning_server src/motion_planning_server.cpp)

# Ajout des dépendances au binaire
ament_target_dependencies(motion_planning_server
  rclcpp
  moveit_core
  moveit_ros_planning
  moveit_ros_planning_interface
  sensor_msgs
  geometry_msgs
  moveit_msgs
)

# Liaison avec les interfaces générées
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
target_link_libraries(motion_planning_server "${cpp_typesupport_target}")

# 4. Installation
install(TARGETS motion_planning_server
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()