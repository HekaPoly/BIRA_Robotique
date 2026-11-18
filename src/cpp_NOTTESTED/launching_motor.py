from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():

    port = LaunchConfiguration('port')

    return LaunchDescription([

        DeclareLaunchArgument(
            'port',
            default_value='/dev/ttyUSB0'
        ),

        Node(
            package='bira_motor_control',
            executable='motor_control_node',
            name='motor_control_node',
            output='screen'
        ),

        Node(
            package='bira_motor_control',
            executable='uart_node',
            name='uart_node',
            output='screen',
            parameters=[{'port': port}]
        )

    ])