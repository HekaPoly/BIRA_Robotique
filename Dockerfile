FROM tiryoh/ros2-desktop-vnc:humble

# Outils de base + MoveIt 2
RUN apt-get update && apt-get install -y \
    ros-humble-moveit \
    ros-humble-moveit-visual-tools \
    python3-rosdep \
    && rm -rf /var/lib/apt/lists/*

# Dépendances déclarées dans les package.xml du repo
COPY src /tmp/src
RUN apt-get update \
    && (rosdep init || true) \
    && rosdep update --rosdistro humble \
    && rosdep install --from-paths /tmp/src --ignore-src --rosdistro humble -y \
    && rm -rf /var/lib/apt/lists/* /tmp/src

# Libs du tuto
RUN apt-get update && apt-get install -y \
    ros-humble-turtlesim \
    && rm -rf /var/lib/apt/lists/*

# Source ROS 2
RUN echo "source /opt/ros/humble/setup.bash" >> /etc/bash.bashrc
