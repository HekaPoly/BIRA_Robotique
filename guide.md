# BIRA Robotique — Environnement ROS 2 & MoveIt

Ce dépôt utilise Docker pour fournir un environnement de développement reproductible basé sur **ROS 2 Humble**, **MoveIt 2** et la configuration du robot **Panda**.

## 1. Prérequis

Installer les éléments suivants sur votre machine :

- Git
- Docker Engine
- Docker Compose (plugin `docker compose`)

La configuration graphique actuelle utilise XWayland pour afficher RViz sur Ubuntu avec une session Wayland.

## 2. Cloner le dépôt et se de déplacer dans la bonne branche

```bash
git clone <URL_DU_DEPOT>
cd BIRA_Robotique 
git fetch origin feat/trajectoire
git checkout feat/trajectoire
```

## 3. Construire et démarrer le conteneur

Autoriser Docker à afficher les applications graphiques sur le bureau Ubuntu :

```bash
xhost +si:localuser:root
```

Construire l'image et démarrer le conteneur :

```bash
docker compose up -d --build
```

## 4. Accéder à l'environnement ROS 2

Ouvrir un terminal dans le conteneur :

```bash
docker compose exec moveit bash
```

ROS 2 Humble est automatiquement initialisé à l'ouverture du terminal.

Vérifier l'installation :

```bash
echo $ROS_DISTRO
ros2 pkg list | grep moveit
ros2 pkg prefix moveit_resources_panda_moveit_config
```

La première commande devrait afficher `humble`.

## 5. Lancer RViz

Depuis le terminal du conteneur :

```bash
rviz2
```

RViz devrait s'ouvrir sur le bureau de votre machine.

## 6. Commandes utiles

| Action | Commande |
|---|---|
| Démarrer le conteneur | `docker compose up -d` |
| Accéder au conteneur | `docker compose exec moveit bash` |
| Reconstruire l'image | `docker compose up -d --build` |
| Arrêter le conteneur | `docker compose down` |

## Notes

- Le dossier du dépôt est monté dans le conteneur : les modifications apportées au code sont conservées sur votre machine.
- Les dossiers `build/`, `install/` et `log/` sont des fichiers générés par ROS 2 et ne doivent pas être suivis par Git.
- La commande `xhost +si:localuser:root` autorise l'accès graphique au conteneur. Pour retirer cette autorisation :

```bash
xhost -si:localuser:root
```

## 7. Rouler la Demo de moveit 

```bash
ros2 launch moveit_resources_panda_moveit_config demo.launch.py 
```

## 8. Comprendre la démo MoveIt et son installation

La démo Panda que nous utilisons provient du package :

```bash
moveit_resources_panda_moveit_config
```

Ce package est installé **à l'échelle du système dans le conteneur Docker**, sous :

```text
/opt/ros/humble/share/moveit_resources_panda_moveit_config
```

Il ne fait donc pas partie de notre dépôt Git et nous n'avons pas besoin de le copier dans notre workspace.

Il fournit notamment :

- Le modèle géométrique du Panda (`panda.urdf.xacro`).
- La description sémantique du robot (`panda.srdf`).
- Les paramètres de cinématique (`kinematics.yaml`).
- Les paramètres des planificateurs.
- Le fichier de lancement de la démo (`demo.launch.py`).

Pour consulter l'emplacement du package :

```bash
ros2 pkg prefix --share moveit_resources_panda_moveit_config
```

### 8.1 Lancer la démo officielle

Dans un premier terminal du conteneur :

```bash
ros2 launch moveit_resources_panda_moveit_config demo.launch.py
```

Cette commande lance plusieurs composants ROS 2, notamment :

- **Move Group** : le serveur central de MoveIt. Il reçoit les requêtes de planification et d'exécution.
- **RViz** : l'interface graphique permettant de visualiser le robot et les trajectoires.
- **Robot State Publisher** : publie les transformations correspondant à l'état du robot.
- **ros2_control** : fournit les interfaces de contrôle utilisées par la démo.

La démo utilise une configuration de contrôle simulée (*mock hardware*). Elle permet donc d'expérimenter la planification et l'exécution dans l'environnement de démonstration, sans commander un véritable robot Panda.

---

## 9. Notre propre package : `hello_moveit`

Nous avons créé notre propre package ROS 2 afin de développer et tester du code C++ utilisant l'API MoveIt.

Il se trouve dans :

```text
src/hello_moveit/
```

Sa structure est organisée ainsi :

```text
hello_moveit/
├── CMakeLists.txt
├── package.xml
├── launch/
│   └── hello_moveit.launch.py
└── src/
    ├── hello_moveit.cpp
    └── experiments.cpp
```

### 9.1 Rôle des différents fichiers

| Fichier | Rôle |
|---|---|
| `package.xml` | Déclare les dépendances ROS 2 du package. |
| `CMakeLists.txt` | Configure la compilation et l'installation des exécutables et fichiers de lancement. |
| `hello_moveit.cpp` | Contient le point d'entrée de notre application C++. |
| `experiments.cpp` | Contient du code expérimental pour explorer différentes fonctionnalités de MoveIt. |
| `hello_moveit.launch.py` | Lance notre nœud en lui fournissant la configuration du robot. |

### 9.2 Pourquoi avons-nous notre propre fichier de lancement ?

Notre application utilise `MoveGroupInterface`, une interface C++ qui permet à un nœud ROS 2 de communiquer avec MoveIt.

Pour construire le modèle du robot, notre nœud doit notamment recevoir :

- `robot_description` : la description physique du robot (URDF).
- `robot_description_semantic` : la description sémantique du robot (SRDF).
- Les paramètres de cinématique nécessaires à la résolution de la cinématique inverse.

Ces paramètres ne sont pas automatiquement partagés entre les nœuds ROS 2. Le fait que la démo officielle soit déjà lancée ne signifie donc pas que notre propre nœud possède automatiquement sa configuration.

Notre fichier `hello_moveit.launch.py` utilise `MoveItConfigsBuilder` pour charger la configuration Panda installée avec ROS 2, puis la transmettre à notre nœud.

Cela nous permet de réutiliser la configuration officielle plutôt que de maintenir nos propres copies des fichiers URDF, SRDF et YAML.

**Important :** notre fichier de lancement démarre uniquement notre application. Il ne démarre pas un deuxième serveur `move_group`. Celui-ci est déjà lancé par la démo officielle.

---

## 10. Compiler et lancer notre application

### 10.1 Compiler le package

Depuis la racine du workspace, dans le conteneur :

```bash
cd /root/ws_moveit

colcon build --packages-select hello_moveit
```

Après la compilation, charger l'environnement du workspace :

```bash
source install/setup.bash
```

Cette dernière commande permet à ROS 2 de découvrir notre package et ses exécutables compilés.

### 10.2 Lancer la démo officielle

Dans un premier terminal :

```bash
docker compose exec moveit bash
```

Puis :

```bash
ros2 launch moveit_resources_panda_moveit_config demo.launch.py
```

Laisser ce terminal ouvert. RViz et le serveur MoveIt doivent rester actifs.

### 10.3 Lancer notre propre nœud

Ouvrir un deuxième terminal dans le conteneur :

```bash
docker compose exec moveit bash
```

Charger l'environnement du workspace :

```bash
source /root/ws_moveit/install/setup.bash
```

Lancer notre application :

```bash
ros2 launch hello_moveit hello_moveit.launch.py
```

Notre nœud reçoit alors la configuration Panda et peut communiquer avec le serveur MoveIt déjà actif.

---

## 11. Observer les résultats dans RViz

Une fois les deux terminaux lancés :

1. Le premier terminal fait fonctionner la démo officielle et son serveur MoveIt.
2. Le deuxième terminal exécute notre propre application C++.
3. Notre application peut envoyer des requêtes de planification au serveur MoveIt.
4. Les résultats de planification et les changements d'état peuvent être visualisés dans RViz.

Pour nos premiers tests, nous explorons notamment :

- La récupération de l'état courant du robot.
- La définition d'une pose cible.
- La résolution de la cinématique inverse (*Inverse Kinematics*).
- La planification en espace articulaire (*Joint Space Planning*).
- La planification cartésienne (*Cartesian Path Planning*).

**À retenir :** RViz est notre outil de visualisation. La planification est effectuée par MoveIt, tandis que notre application C++ sert à définir les objectifs et à envoyer les requêtes correspondantes.

---

## 12. Résumé de l'architecture

Notre environnement fonctionne avec trois éléments principaux :

```text
┌──────────────────────────────────────────────┐
│                 Docker                       │
│              ROS 2 Humble                    │
│                                              │
│  ┌──────────────────┐  ┌───────────────────┐ │
│  │ Démo officielle  │  │ Notre application │ │
│  │ Panda MoveIt     │  │ hello_moveit      │ │
│  │                  │  │                   │ │
│  │ - move_group     │◄─┤ - MoveGroup       │ │
│  │ - RViz           │  │   Interface       │ │
│  │ - ros2_control   │  │ - Code C++        │ │
│  └──────────────────┘  └───────────────────┘ │
│            ▲                    ▲            │
│            │                    │            │
│  Configuration Panda     Configuration      │
│  installée dans ROS      transmise par      │
│  /opt/ros/humble         notre launch file   │
└──────────────────────────────────────────────┘
```

L'objectif de cette architecture est de séparer clairement :

- **La configuration du robot** : fournie par le package Panda.
- **Le serveur de planification** : fourni par MoveIt.
- **Notre logique applicative** : développée dans `hello_moveit`.

Cette séparation nous permettra progressivement de développer nos propres méthodes de planification et de génération de trajectoires, puis de les adapter à notre robot réel.