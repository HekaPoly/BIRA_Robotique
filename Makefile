# Variables
COMPOSE_FILE=docker-compose.yml

.PHONY: build up start enter stop down clean

# 1. Build the image (Run this the very first time or after changing Dockerfile)
build:
	docker compose build

# 2. Start the container in the background and open GUI permissions
up:
	xhost +local:docker
	docker compose up -d

# 3. Enter the container terminal
enter:
	docker compose exec ros_dev bash

# 4. Stop the container and lock GUI permissions
stop:
	docker compose stop
	xhost -local:docker

# 5. Bring everything down (removes the container, but keeps the image)
down:
	docker compose down
	xhost -local:docker

# 6. Full Cleanup (Removes container and the custom image)
clean:
	docker compose down --rmi all
	xhost -local:docker

# 7. One-command Shortcut: Build, Start, and Enter
all:
	xhost +local:docker
	docker compose up -d --build
	docker compose exec ros_dev bash