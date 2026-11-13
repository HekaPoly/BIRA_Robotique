FROM osrf/ros:humble-desktop

# Install the tutorial tools and MoveIt 2 so they are always there
RUN apt-get update && apt-get install -y \
    ros-humble-turtlesim \
    ros-humble-moveit \
    ros-humble-moveit-visual-tools \
    ros-humble-rviz-visual-tools \
    ros-humble-tf2-geometry-msgs \
    ros-humble-interactive-markers \
    ros-humble-rmw-cyclonedds-cpp \
    ros-humble-moveit-resources-panda-description \
    ros-humble-moveit-resources-panda-moveit-config \
    python3-colcon-common-extensions \
    python3-colcon-mixin \
    python3-rosdep \
    python3-vcstool \
    git \
    && rm -rf /var/lib/apt/lists/*  

RUN echo "source /opt/ros/humble/setup.bash" > /root/.bashrc

# Install and update the default colcon mixins
RUN colcon mixin update default