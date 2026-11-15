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