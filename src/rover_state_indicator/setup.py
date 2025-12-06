from setuptools import setup

package_name = 'rover_state_indicator'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
         ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ndari',
    maintainer_email='ndaribikearley@gmail.com',
    description='SR-32 – Rover LED indicator node for state feedback.',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'indicator = rover_state_indicator.indicator:main',
        ],
    },
)

