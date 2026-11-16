
import os

from launch import LaunchDescription
from launch_ros.actions import Node

from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():

    # Load the same Panda configuration used by the official demo
    moveit_config = (
        MoveItConfigsBuilder("moveit_resources_panda")
        .robot_description(
            file_path="config/panda.urdf.xacro"
        )
        .robot_description_semantic(
            file_path="config/panda.srdf"
        )
        .to_moveit_configs()
    )

    # Start our own C++ application with the robot configuration
    hello_moveit_node = Node(
        package="hello_moveit",
        executable="hello_moveit",
        name="hello_moveit",
        output="screen",
        parameters=[
            moveit_config.to_dict()
        ],
    )

    return LaunchDescription([
        hello_moveit_node
    ])
