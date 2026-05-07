# BIRA_Robotique
Code source de l’équipe logiciel/IA pour la partie robotique du projet BIRA

# Structure de fichiers
```
.
├── build/
├── log/
├── install/
└── src/
    ├── motion_planning_api/
    │   ├── include
    │   ├── launch/
    │   │   └── launchFile
    │   ├── src/
    │   │   └── code source (Où l'implémentation du Node Ros est)
    │   ├── CMakeLists.txt (Définit comment compiler ce package)
    │   └── package.xml (Décrit le package et ses dépendances)
    └── D'autres packages éventuellement/
```
Les dossiers `build`, `install` et `log` sont dans le `.gitignore`, donc ils ne seront pas visibles. Ils sont générés durant la compilation ( •̀ ω •́ )✧

# Étapes pour rouler le code
## 1. Compiler MoveIt
**À FAIRE SEULEMENT S'IL Y A DES ERREURS AVEC MOVEIT**<br>
1. Aller dans le dossier ~/ws_moveit/
2. Source ROS2: `source /opt/ros/humble/setup.sh`
3. Exécuter la commande `colcon build --mixin release --packages-skip moveit_ros_perception --parallel-workers=2`. Ici, on ne compile pas moveit_ros_perception à cause de problèmes de dépendance avec OpenCV (On n'en a pas besoin anyway ο(=•ω＜=)ρ⌒☆). On compile en limitant le nombre de "jobs" à 2 afin de limiter la consommation en RAM.

S'il y a des erreurs de dépendance, refaire les étapes précédentes en exécutant les commandes suivantes avant l'étape 2 et après l'étape 1:
```
rosdep update
rosdep install -r --from-paths ~/ws_moveit/src --ignore-src --rosdistro $ROS_DISTRO -y
```
## 2. Compiler et exécuter notre code
1. Aller dans le dossier ~/Desktop/BIRA_Robotique/ (ce dossier est un workspace ROS2)
2. Source ROS2 si ce n'est pas encore fait: `source /opt/ros/humble/setup.sh`
3. Source le workspace moveit: `source ~/ws_moveit/install/setup.sh`
4. Compiler notre workspace: `colcon build`
5. Source notre workspace: `source ./install/setup.sh`
6. Launch le move_group: `ros2 launch moveit2_tutorials move_group.launch.py`. Le Node MoveGroupInterface permet d'exécuter les fonctions principales de MoveIt facilement.
7. Launch notre Node: `ros2 launch motion_planning_api motion_planning_api_tutorial.launch.py`

ᕦ(ò_óˇ)ᕤ

## Feature cpp-server

Ce code tourne sur un docker que j'ai créé qu'on peut trouver ici : https://github.com/mtt-ben/ROS2_tutorial
Le Dockerfile précise toutes les dépendances à installer

1. Aller dans le dossier du workspace
2. Source ROS2: `source /opt/ros/humble/setup.sh`
3. Build le node: `colcon build --packages-select bira_motion_planning --symlink-install`
4. source notre workspace: `source install/setup.bash`
5. Launch le node: `ros2 launch bira_motion_planning motion_planning.launch.py`
6. Tester dans un autre terminal (où ROS2 et notre workspace sont source): 
ros2 service call /plan_kinematic_path bira_motion_planning/srv/PlanTrajectory "{
  start_joints: {
    name: [
      'joint1', 
      'joint2', 
      'joint3', 
      'joint4', 
      'joint5', 
      'joint6'
    ],
    position: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  },
  final_pos: {
    header: {
      frame_id: 'meca_base_link'
    },
    pose: {
      position: {x: 0.2, y: 0.0, z: 0.2},
      orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}
    }
  }
}"

The meca500 urdf comes from : https://github.com/Vanderbilt-Applied-Robotics-Lab/meca500_ros2

Next : add group_link  