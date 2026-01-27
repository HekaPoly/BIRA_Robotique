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