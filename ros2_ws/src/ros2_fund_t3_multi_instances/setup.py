import os
from glob import glob

from setuptools import find_packages, setup

package_name = 'ros2_fund_t3_multi_instances'

setup(
    name=package_name,
    version='1.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # Include all launch files.
        (os.path.join('share', package_name, 'launch'), glob('launch/*'))        
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='benjamincanton',
    maintainer_email='benjamin.canton@htecgroup.com',
    description='ROS2 Fundamentals - Task 3: Multi Instances. Python',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'pub3_cust_msg=ros2_fund_t3_multi_instances.publisher3_cust_msg:main',
            'sub3_cust_msg=ros2_fund_t3_multi_instances.subscriber3_cust_msg:main',
        ],
    },
)