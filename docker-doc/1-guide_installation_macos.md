# Installation de l'environnement ROS2 — macOS

## 1. Vérifier les prérequis

- macOS récent (Intel ou Apple Silicon M1/M2/M3 — les deux fonctionnent).

## 2. Installer Docker Desktop

1. Téléchargez Docker Desktop depuis [docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop/) (choisissez la version "Apple Silicon" ou "Intel chip" selon votre Mac).
2. Ouvrez le fichier `.dmg` téléchargé et glissez Docker dans le dossier Applications.
3. Lancez Docker Desktop depuis Applications, acceptez les autorisations demandées.
4. Attendez que l'icône dans la barre de menu indique que Docker est prêt (généralement quelques dizaines de secondes au premier démarrage).
5. Vérifiez que tout fonctionne.

**[Dans le Terminal]** :
```bash
docker --version
docker run hello-world
```
Si un message de bienvenue s'affiche, Docker fonctionne correctement.

## 3. Lancer l'environnement

**[Dans un terminal, dans le dossier `BIRA_Robotique`]** :
```bash
docker compose up -d --build
```

La première exécution va télécharger et construire l'image (plusieurs Go, ça peut prendre une dizaine de minutes selon la connexion). Les fois suivantes, le démarrage sera quasi instantané.

Vérifiez que le conteneur tourne bien :
```bash
docker ps
```
Vous devriez voir `ros2_humble_desktop` avec un statut "Up".

Une fois le conteneur lancé, ouvrez [http://localhost:6080](http://localhost:6080) dans votre navigateur pour accéder au bureau ROS2.
