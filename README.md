# BIRA_Robotique
Code source de l’équipe logiciel/IA pour la partie robotique du projet BIRA

# Structure de fichiers
```
.
├── build/
├── log/
├── install/
└── src/
    ├── bira_motion_planning/ (Serveur de planification C++)
    │   ├── launch/
    │   │   └── motion_planning.launch.py
    │   ├── src/
    │   │   └── motion_planning_server.cpp (Où l'implémentation du Node Ros est)
    │   ├── CMakeLists.txt (Définit comment compiler ce package)
    │   └── package.xml (Décrit le package et ses dépendances)
    ├── bira_msgs/ (Définition du service PlanTrajectory)
    ├── robot_client/ (Client Python qui appelle le serveur)
    ├── meca500_description/ (URDF et meshes du Meca500)
    └── meca500_moveit/ (Configuration MoveIt du Meca500)
```
Les dossiers `build`, `install` et `log` sont dans le `.gitignore`, donc ils ne seront pas visibles. Ils sont générés durant la compilation ( •̀ ω •́ )✧

# Étapes pour rouler le code
Tout tourne sur le docker qu'on peut trouver ici : https://github.com/mtt-ben/ROS2_tutorial. Le Dockerfile installe toutes les dépendances (ROS2 Humble, MoveIt déjà compilé), pas besoin de recompiler MoveIt.

## 1. Compiler notre code
1. Aller dans le dossier du workspace (ce dossier est un workspace ROS2)
2. Source ROS2: `source /opt/ros/humble/setup.sh`
3. Compiler notre workspace: `colcon build --symlink-install`

## 2. Lancer le serveur et le client
Dans chaque terminal, source ROS2 et notre workspace: `source /opt/ros/humble/setup.sh && source install/setup.bash`

- Terminal 1, le serveur de planification: `ros2 launch bira_motion_planning motion_planning.launch.py`
- Terminal 2, le client: `python3 src/robot_client/robot_client/robot_client.py`

Le client attend que le service `/plan_kinematic_path` soit dispo, envoie la requête et affiche la trajectoire reçue.

ᕦ(ò_óˇ)ᕤ

## 3. Tester
Dans un autre terminal (où ROS2 et notre workspace sont source):
```
ros2 node list                                        # doit afficher /motion_planning_server
ros2 service list | grep plan_kinematic_path          # doit afficher /plan_kinematic_path
ros2 interface show bira_msgs/srv/PlanTrajectory      # affiche la définition du service
```
Appeler le service à la main, sans le client:
```
ros2 service call /plan_kinematic_path bira_msgs/srv/PlanTrajectory "{
  start_joints: {
    name: ['joint1', 'joint2', 'joint3', 'joint4', 'joint5', 'joint6'],
    position: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
  },
  final_pos: {
    header: {frame_id: 'meca_base_link'},
    pose: {
      position: {x: 0.2, y: 0.0, z: 0.2},
      orientation: {x: 0.0, y: 0.0, z: 0.0, w: 1.0}
    }
  }
}"
```

The meca500 urdf comes from : https://github.com/Vanderbilt-Applied-Robotics-Lab/meca500_ros2