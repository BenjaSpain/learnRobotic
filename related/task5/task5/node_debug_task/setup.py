import os

from setuptools import find_packages, setup
from glob import glob

package_name = 'node_debug_task'

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
    maintainer='root',
    maintainer_email='haris.sikic@htecgroup.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'node_a = node_debug_task.NodeA:main',
            'node_b = node_debug_task.NodeB:main',
            'node_c = node_debug_task.NodeC:main',
            'node_d = node_debug_task.NodeD:main',
            'node_s = node_debug_task.NodeS:main',
        ],
    },
)
