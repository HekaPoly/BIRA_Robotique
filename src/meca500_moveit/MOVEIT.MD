# Documentation : meca500_moveit

## 1. Démarrage rapide

**Prérequis :** ROS 2 avec MoveIt 2 installé, et les packages externes listés au §4 (`meca500_description`, plugin matériel).

**Construire :**

```bash
colcon build --packages-select meca500_moveit
source install/setup.bash
```

**Ouvrir la configuration dans le Setup Assistant :**

```bash
ros2 launch meca500_moveit setup_assistant.launch.py
```
## 2. Vue d'ensemble


`meca500_moveit` est un **package ROS 2 de configuration** pour **MoveIt 2**, dédié au bras.

Il a trois utiltés :

| Utilité | Mécanisme MoveIt | Configuré par |
|---|---|---|
| Planifier un mouvement avec évitement d'obstacles | `move_group` + OMPL | `ompl_planning.yaml`, `kinematics.yaml` |
| Planifier un mouvement en ligne droite / cartésien | `move_group` + Pilz | `pilz_cartesian_limits.yaml` |
| Piloter en temps réel | MoveIt Servo | `servo.yaml` |

## 3. Composants et responsabilités

Les fichiers sont regroupés par **responsabilité**.

### 3.1 Modèle du robot

| Fichier | Responsabilité |
|---|---|
| `meca500.urdf.xacro` | Point d'entrée du modèle : trouve `meca500_description`. |
| `meca500.srdf` | Sémantique MoveIt ajoutée au modèle : groupe de planification `meca500_arm`, pose nommée `home`, liaison fixe `world` → `meca_base_link`, paires de links dont la collision est ignorée. |
| `meca500.ros2_control.xacro` | Déclare le robot à `ros2_control` : joints, interfaces de commande et d'état en position, paramètres de connexion (IP, port) et plugin matériel. |
| `initial_positions.yaml` | Position de départ des joints, utilisée par le matériel simulé. |

### 3.2 Planification

| Fichier | Responsabilité |
|---|---|
| `kinematics.yaml` | Choix du solveur de cinématique inverse du groupe `meca500_arm` (KDL). |
| `ompl_planning.yaml` | Choix des planificateurs OMPL et du planificateur par défaut. |
| `pilz_cartesian_limits.yaml` | Limites cartésiennes pour les trajectoires Pilz. |
| `joint_limits.yaml` | Limites de vitesse et d'accélération par joint, et facteurs de réduction par défaut. Utilisé par tous les planificateurs. |

### 3.3 Exécution (contrôle)

| Fichier | Responsabilité |
|---|---|
| `ros2_controllers.yaml` | Côté **ros2_control** : les contrôleurs qui existent et leurs joints/interfaces. |
| `moveit_controllers.yaml` | Côté **MoveIt** : à quel contrôleur envoyer les trajectoires, via quelle action. |

### 3.4 Pilotage temps réel

| Fichier | Responsabilité |
|---|---|
| `servo.yaml` | Paramètres de MoveIt Servo : repères, groupe, topics d'entrée/sortie, seuils de singularité, vérification de collisions. |

### 3.5 Outil "setup"

| Fichier | Responsabilité |
|---|---|
| `launch/setup_assistant.launch.py` | Ouvre cette configuration dans le Setup Assistant. |