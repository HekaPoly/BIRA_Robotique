#!/usr/bin/env python3
import os
import yaml
import tempfile
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("meca500", package_name="meca500_moveit")
        .robot_description(file_path="config/meca500.urdf.xacro")
        .robot_description_semantic(file_path="config/meca500.srdf")
        .to_moveit_configs()
    )

    # Écrit les params du planner dans un fichier yaml temporaire
    planner_params = {
        "motion_planning_server": {
            "ros__parameters": {
                "planning_plugin": "ompl_interface/OMPLPlanner",
                "request_adapters": (
                    "default_planner_request_adapters/AddTimeOptimalParameterization "
                    "default_planner_request_adapters/FixWorkspaceBounds "
                    "default_planner_request_adapters/FixStartStateBounds "
                    "default_planner_request_adapters/FixStartStateCollision "
                    "default_planner_request_adapters/FixStartStatePathConstraints"
                ),
            }
        }
    }
    with tempfile.NamedTemporaryFile(mode="w", suffix=".yaml", delete=False) as f:
        yaml.dump(planner_params, f)
        planner_params_path = f.name
    
    print(f"[DEBUG] Planner params file: {planner_params_path}")
    with open(planner_params_path) as f:
        print(f.read())

    planning_server_node = Node(
        package="bira_motion_planning",
        executable="motion_planning_server",
        name="motion_planning_server",
        output="screen",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.joint_limits,
            planner_params_path,  # fichier yaml avec le bon namespace
        ],
    )

    return LaunchDescription([planning_server_node])