# learnRobotic
Robotic basic learning

Steps:
1. Install Ubuntu: (Jazzy or later), host or container
2. Install ROS2 Jazzy Distribution
3. '1_ROS2_basics.md'

## Install ROS2 Jazzy Distribution
[Installation Documentation](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html)
```bash
# Ensure Ubuntu Universe repository is ENABLED 
sudo apt update 
sudo apt install software-properties-common 

sudo add-apt-repository universe 
 
# Install: ros2-apt-source package -> Configure ROS2 repositories for system 
sudo apt update && sudo apt install curl -y 
export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F'"' '{print $4}') 
curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb" 
sudo dpkg -i /tmp/ros2-apt-source.deb 
 
# Install development tools (optional) -> To build ROS packages for development 
sudo apt update && sudo apt install ros-dev-tools 
 
# sudo apt update && sudo apt upgrade -> SKIPPPED TO AVOID OS MISSWORKING. Upgraded a few months ago, Jazzy should be older than last upgrade 


# Install Desktop: ROS, RViz, demos, tutorials 
sudo apt update && sudo apt install ros-jazzy-desktop 
 
# ROS-Base Install (Bare Bones) -> Skipped (installed Desktop tools) 
# sudo apt update && sudo apt install ros-jazzy-ros-base 
 
# Set up your environment by sourcing your environ 
source /opt/ros/jazzy/setup.bash 
```

