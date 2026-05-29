from launch import LaunchDescription
from launch.actions import GroupAction, IncludeLaunchDescription
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import PushROSNamespace


def generate_launch_description():
    launch_dir = PathJoinSubstitution([FindPackageShare('py_launch_tutorial'), 'launch'])

    return LaunchDescription([
        GroupAction(
            actions=[
                PushROSNamespace('turtlesim2'),
                IncludeLaunchDescription(PathJoinSubstitution([launch_dir, 'turtlesim_world_2_launch.py'])),
            ]
        ),
    ])