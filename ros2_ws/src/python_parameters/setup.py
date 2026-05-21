from setuptools import find_packages, setup

package_name = 'python_parameters'

setup(
    name=package_name,
    version='0.0.1',
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
    description='Python parameter tutorial',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'minimal_param_node = python_parameters.python_parameters_node:main',
        ],
    },
)
