import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder

def generate_launch_description():
    # 1. Chargement automatique des configs MoveIt du Panda
    moveit_config = (
        MoveItConfigsBuilder("panda", package_name="moveit_resources_panda_moveit_config")
        .robot_description(file_path="config/panda.urdf.xacro")
        .robot_description_semantic(file_path="config/panda.srdf")
        .planning_pipelines(pipelines=["ompl"]) # Charge les paramètres OMPL
        .to_moveit_configs()
    )

    # 2. Définition du nœud Serveur
    planning_server_node = Node(
        package="bira_motion_planning",
        executable="motion_planning_server",
        name="motion_planning_server",
        output="screen",
        parameters=[
            moveit_config.to_dict(),
            {
                "ompl.planning_plugin": "ompl_interface/OMPLPlanner",
                "ompl.request_adapters": (
                    "default_planner_request_adapters/AddTimeOptimalParameterization "
                    "default_planner_request_adapters/FixWorkspaceBounds "
                    "default_planner_request_adapters/FixStartStateBounds "
                    "default_planner_request_adapters/FixStartStateCollision "
                    "default_planner_request_adapters/FixStartStatePathConstraints"
                ),
                # On définit aussi les paramètres globaux au cas où
                "planning_plugin": "ompl_interface/OMPLPlanner",
                "planning_pipelines": ["ompl"],
                "default_planning_pipeline": "ompl",
            },
        ],
    )

    return LaunchDescription([planning_server_node])