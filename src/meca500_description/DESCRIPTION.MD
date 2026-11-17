# Documentation : meca500_description

## 1. Démarrage rapide

**Prérequis :** ROS 2 avec `xacro` installé.

**Construire :**

```bash
colcon build --packages-select meca500_description
source install/setup.bash
```

**Vérifier le modèle :**

```bash
xacro $(ros2 pkg prefix meca500_description)/share/meca500_description/urdf/meca500.xacro > meca500.urdf
check_urdf meca500.urdf
```

## 2. Vue d'ensemble

`meca500_description` est un **package ROS 2 de description** : il contient le **modèle géométrique** du bras BIRA (URDF + meshes)

Il a trois utilités :

| Utilité | Utilisé par | Fourni par |
|---|---|---|
| Afficher le robot | RViz, `robot_state_publisher` | `meshes/visual/*.dae` |
| Détecter les collisions | MoveIt (planification, Servo) | `meshes/collision/*.stl` |
| Calculer la cinématique (position des links, limites des joints) | MoveIt, `ros2_control` | `urdf/meca500.xacro` |

Il est un dépendant de meca500_moveit (`config/meca500.urdf.xacro`) et déclaré comme dépendance par `bira_motion_planning`.

## 3. Composants et responsabilités

Les fichiers sont regroupés par **responsabilité**.

### 3.1 Modèle du robot

| Fichier | Responsabilité |
|---|---|
| `urdf/meca500.xacro` | Modèle du robot : inclut les links (`meca_base_link` → `meca_axis_6_link`) et les joints rotatifs (`joint1` → `joint6`) avec leurs origines, axes et limites.

### 3.2 Géométrie

| Fichier | Responsabilité |
|---|---|
| `meshes/visual/meca_500_r3_*.dae` | Apparence de chaque link (base, j1 à j6), pour l'affichage. |
| `meshes/collision/meca_500_r3_*.stl` | Forme de chaque link (base, j1 à j6), pour les calculs de collision. |