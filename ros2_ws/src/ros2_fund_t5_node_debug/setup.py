import os

from setuptools import find_packages, setup
from glob import glob

#package_name = 'node_debug_task'
package_name = 'ros2_fund_t5_node_debug'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'config'), glob('config/*.yaml')),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py'))
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='benjamincanton',
    maintainer_email='benjamin.canton@htecgroup.com',
    description='ROS2 Fundamentals - Task 5: Node Debugging. C++',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'node_a = ros2_fund_t5_node_debug.NodeA:main',
            'node_b = ros2_fund_t5_node_debug.NodeB:main',
            'node_c = ros2_fund_t5_node_debug.NodeC:main',
            'node_d = ros2_fund_t5_node_debug.NodeD:main',
            'node_s = ros2_fund_t5_node_debug.NodeS:main',
        ],
    },
)
