from setuptools import find_packages, setup

package_name = 'ros2_fund_t1_pub1_sub1'

setup(
    name=package_name,
    version='2.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='benjamincanton',
    maintainer_email='benjamin.canton@htecgroup.com',
    description='ROS2 Fundamentals - Task 1: Single Publisher-Single Subscriber. Python - Modified for Task2',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'publisher1=ros2_fund_t1_pub1_sub1.publisher1:main',
            'subscriber1=ros2_fund_t1_pub1_sub1.subscriber1:main',
            'pub1_cust_msg=ros2_fund_t1_pub1_sub1.publisher1_cust_msg:main',
            'sub1_cust_msg=ros2_fund_t1_pub1_sub1.subscriber1_cust_msg:main',
        ],
    },
)