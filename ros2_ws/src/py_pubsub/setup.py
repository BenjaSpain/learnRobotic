from setuptools import find_packages, setup

package_name = 'py_pubsub'

setup(
    name=package_name,
    version='0.0.2  ',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='benjamincanton',
    maintainer_email='55950461+BenjaSpain@users.noreply.github.com',
    description='Examples of minimal publisher/subscriber using rclpy and tutorial_interfaces.msg.Num',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'talker = py_pubsub.publisher_member_function:main',
            'listener = py_pubsub.subscriber_member_function:main',            
            'talker_interf = py_pubsub.publisher_member_function_interf:main',
            'listener_interf = py_pubsub.subscriber_member_function_interf:main',          
        ],
    },
)
