# Installation de l'environnement ROS2 — Linux

Bon si vous avez Linux je me doute que ca va vous servir à rien, mais on sait jamais.

## 1. Vérifier les prérequis

- Une distribution Linux avec accès `sudo` (Ubuntu/Debian utilisé comme référence ci-dessous ; adaptez la commande d'installation si vous êtes sur une autre distro).

## 2. Installer Docker Engine

**[Dans un terminal]** :
```bash
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER
```

Déconnectez-vous/reconnectez-vous (ou redémarrez) pour que l'ajout au groupe `docker` prenne effet.

Vérifiez que tout fonctionne :
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
