import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    config_s = os.path.join(
        get_package_share_directory('node_debug_task'),
        'config',
        'params_s.yaml'
    )

    config_a = os.path.join(
        get_package_share_directory('node_debug_task'),
        'config',
        'params_a.yaml'
    )

    config_b = os.path.join(
        get_package_share_directory('node_debug_task'),
        'config',
        'params_a.yaml'
    )

    config_d = os.path.join(
        get_package_share_directory('node_debug_task'),
        'config',
        'params_d.yaml'
    )

    return LaunchDescription([
        Node(
            package='node_debug_task',
            executable='node_s',
            name='NodeS',
            parameters=[config_s],
            output='screen'
        ),
        Node(
            package='node_debug_task',
            executable='node_a',
            namespace='N',
            name='NodeA',
            parameters=[config_a],
            remappings=[
                ('/SA', 'SA')
            ],
            output='screen'
        ),
        Node(
            package='node_debug_task',
            executable='node_b',
            namespace='N',
            name='NodeB',
            parameters=[config_b],
            output='screen'
        ),
        Node(
            package='node_debug_task',
            executable='node_c',
            namespace='n',
            name='NodeC',
            output='screen'
        ),
        Node(
            package='node_debug_task',
            executable='node_d',
            namespace='N',
            name='NodeD',
            parameters=[config_d],
            output='screen'
        )
    ])
