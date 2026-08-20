IMAGE_NAME ?= joshua

.PHONY: help check setup build build-unified build-amd64 build-arm64 build-server build-pi run clean delete-images up down logs shell

help:
	@echo "Joshua Docker Makefile"
	@echo ""
	@echo "Recommended commands:"
	@echo " make setup - One-command setup (runs setup.sh)"
	@echo " make check - Check Docker prerequisites"
	@echo " make up - Start dev environment (docker-compose)"
	@echo " make shell - Enter dev container"
	@echo " make logs - View container logs"
	@echo " make down - Stop all containers"
	@echo ""
	@echo "Build commands:"
	@echo " make build - Auto-detect platform and build (uses build.sh)"
	@echo " make build-arm64 - Build for ARM64 (Sitara)"
	@echo " make build-amd64 - Build for x86_64"
	@echo ""
	@echo "Legacy commands (use unified instead):"
	@echo " make build-server - Build x86_64 (old Dockerfile.server)"
	@echo " make build-pi - Build ARM64 (old Dockerfile)"

setup:
	@./setup.sh

check:
	@./check-prereqs.sh

build:
	@./build.sh

build-amd64:
	@docker buildx build --platform linux/amd64 --load \
		--build-arg UID=$$(id -u) --build-arg GID=$$(id -g) \
		-f Dockerfile.unified -t $(IMAGE_NAME):amd64 ..

build-arm64:
	@docker buildx build --platform linux/arm64 --load \
		--build-arg UID=$$(id -u) --build-arg GID=$$(id -g) \
		-f Dockerfile.unified -t $(IMAGE_NAME):arm64 ..

up:
	@docker compose -f docker-compose.dev.yml up -d
	@echo "Services started. Enter container with: make shell"

down:
	@docker compose -f docker-compose.dev.yml down

logs:
	@docker compose -f docker-compose.dev.yml logs -f

shell:
	@docker exec -it joshua-dev bash || echo "Container not running. Start with: make up"

build-pi:
	@echo "Building Docker image $(IMAGE_NAME) for Sitara (ARM64)..."
	docker buildx build --platform linux/arm64 --load \
		--build-arg UID=$$(id -u) --build-arg GID=$$(id -g) \
		-t $(IMAGE_NAME) -f Dockerfile ..

# Builds and tags the server image as $(IMAGE_NAME). The top-level Makefile
# passes IMAGE_NAME=$(DOCKER_SERVER_IMAGE) (joshua-dev) so the image Docker
# then runs matches the tag created here.
build-server:
	@echo "Building Docker image $(IMAGE_NAME) for x86 Ubuntu 22.04..."
	docker buildx build --platform linux/amd64 --load \
		--build-arg UID=$$(id -u) --build-arg GID=$$(id -g) \
		-f Dockerfile.server -t $(IMAGE_NAME) ..

run:
	# No --gpus flag: Sitara AM62x has Sitara AM62x (not CUDA)
	docker run -it --rm --hostname "Sitara AM62x" $(IMAGE_NAME)

clean:
	docker rmi $(IMAGE_NAME) || true

delete-images:
	@echo "Warning: This will delete ALL Docker images!"
	@read -p "Are you sure? (y/N) " -n 1 -r; \
	echo; \
	if [[ $$REPLY =~ ^[Yy]$$ ]]; then \
		docker rmi $$(docker images -q) || true; \
	fi
