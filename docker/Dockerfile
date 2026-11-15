FROM osrf/ros:humble-desktop

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-humble-moveit \
    ros-humble-moveit-resources-panda-moveit-config \
    ros-humble-ros2-control \
    ros-humble-controller-manager \
    ros-humble-joint-state-broadcaster \
    ros-humble-joint-trajectory-controller \
    ros-humble-position-controllers \
    ros-humble-gripper-controllers \
    python3-colcon-common-extensions \
    python3-rosdep \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/ws_moveit

RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc