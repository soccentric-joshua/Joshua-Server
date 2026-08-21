# Joshua-Server

## Project Summary

Joshua-Server is the central C++ orchestration and data-collection backend for Joshua IV&V. Its architecture has an embedded Client connecting over gRPC to `HalService`, while the dashboard and external tools use a Drogon REST API on `/api/v1`. The service is designed around PostgreSQL, versioned SQL migrations, a service layer for runs, logs, authentication, and OTA, and repositories for persistence. Optional Prometheus metrics and Docker development assets are also documented.

The source tree contains configuration and logging setup, database clients and migration code, gRPC server runners and service implementations, and stream backpressure support. Environment variables configure gRPC and HTTP listeners, database credentials, session signing, registry location, OTA storage, and log level. The API inventory includes health, login and token creation, module listing, run creation/details, logs and SSE tailing, target heartbeat, OTA upload/trigger, trends, distributions, heatmaps, anomaly detection, run comparison, baseline diffs, and typed thresholds. A YAML module registry and startup migrations define important runtime contracts.

The repository includes CMake presets, Docker Compose, vcpkg configuration, unit tests for services and utilities, migration tests, a config test, and a stream backpressure test. The existing implementation-status notes say database, CRUD, ingest, and gRPC stages are not complete in that plan, so this README does not upgrade them merely because source files exist. Code surfaces are marked partial where tests or status notes do not establish end-to-end behavior. Live PostgreSQL, authenticated REST, embedded-client streaming, OTA storage, and production deployment remain manual validation work.

## Feature Inventory

| # | Name / Description | Implemented? | Test Coverage? | Manual Test |
|---:|---|---|---|---|
| 1 | C++ executable and CMake/vcpkg build | Implemented | Partial; test presets exist | [ ] Manual verification required |
| 2 | Environment and CLI configuration | Implemented | Partial; config test | [ ] Manual verification required |
| 3 | Structured logging and log levels | Implemented | Unknown | [ ] Manual verification required |
| 4 | PostgreSQL client and migrations | Partial; plan marks DB stage incomplete | Migration/config tests | [ ] Manual verification required |
| 5 | gRPC `HalService` listener | Partial; end-to-end flow unestablished | Unknown | [ ] Manual verification required |
| 6 | REST `/api/v1` service | Partial; route surface documented | Partial route tests | [ ] Manual verification required |
| 7 | Sessions and API tokens | Implemented | Partial auth coverage | [ ] Manual verification required |
| 8 | Module registry/listing | Implemented | `ModuleRegistryTest.cc` | [ ] Manual verification required |
| 9 | Run orchestration, ingest, and persistence | Partial; plan identifies unfinished stages | Partial run tests | [ ] Manual verification required |
| 10 | Logs and live SSE tail | Implemented | Unknown integration coverage | [ ] Manual verification required |
| 11 | OTA upload/trigger workflow | Partial | Unknown | [ ] Manual verification required |
| 12 | Trends, distributions, heatmaps, anomalies, comparisons, baselines | Implemented | Partial service tests | [ ] Manual verification required |
| 13 | Typed thresholds and anomaly services | Implemented | `ThresholdServiceTest.cc`, `AnomalyServiceTest.cc` | [ ] Manual verification required |
| 14 | Stream backpressure handling | Implemented | `StreamBackpressureTest.cc` | [ ] Manual verification required |
| 15 | Docker Compose development environment | Implemented | Unknown | [ ] Manual verification required |
| 16 | Optional Prometheus metrics | Partial | Unknown | [ ] Manual verification required |

## Build and Test

```bash
cmake --preset debug && cmake --build build/debug
cmake --preset test && cmake --build build/test
ctest --test-dir build/test --output-on-failure
```

Docker setup is documented in `docker/README.md`; environment defaults are in `.env.example`.# Joshua-Server

## Project Summary

Joshua-Server is the central C++ orchestration and data-collection backend for
Joshua IV&V. Its intended architecture has an embedded Client connecting over
gRPC to `HalService`, while the dashboard and external tools use a Drogon REST
API on `/api/v1`. The service is designed around PostgreSQL, versioned SQL
migrations, a service layer for runs/logs/authentication/OTA, and repositories
for persistence. Optional Prometheus metrics and Docker development assets are
also documented.

The source tree contains configuration and logging setup, database clients and
migration code, gRPC server runners and service implementations, and stream
backpressure support. Environment variables configure gRPC and HTTP listeners,
database credentials, session signing, registry location, OTA storage, and log
level. The API inventory includes health, login and token creation, module
listing, run creation/details, logs and SSE tailing, target heartbeat, OTA
upload/trigger, trends, distributions, heatmaps, anomaly detection, run
comparison, baseline diffs, and typed thresholds. A YAML module registry and
startup migrations define important runtime contracts.

The repository includes CMake presets, Docker Compose, vcpkg configuration,
unit tests for services and utilities, migration tests, a config test, and a
stream backpressure test. The older implementation-status section says the
database, CRUD, ingest, and gRPC stages are not complete in that plan, so this
README does not upgrade them to “implemented” merely because corresponding
source files exist. The feature table records code surfaces as partial where
the checked-in tests or status notes do not establish end-to-end behavior.
Live PostgreSQL, authenticated REST, embedded-client streaming, OTA storage,
and production deployment remain manual validation work.

## Feature Inventory

| # | Name / Description | Implemented? | Test Coverage? | Manual Test |
|---:|---|---|---|---|
| 1 | C++ server executable and CMake/vcpkg build | Implemented | Partial; CMake test presets exist | [ ] Manual verification required |
| 2 | Configuration from environment and CLI flags | Implemented | Partial; config test exists | [ ] Manual verification required |
| 3 | Structured logging and log-level control | Implemented | Unknown | [ ] Manual verification required |
| 4 | PostgreSQL database client and schema migrations | Partial; implementation and migration tests exist, plan marks DB stage incomplete | Covered by migration/config tests | [ ] Manual verification required |
| 5 | gRPC `HalService` listener | Partial; server surface exists, end-to-end client flow is not established | Unknown | [ ] Manual verification required |
| 6 | REST `/api/v1` service | Partial; route/service surface is documented | Partial; route-specific tests vary | [ ] Manual verification required |
| 7 | Authentication sessions and API tokens | Implemented | Partial; auth-related service tests exist | [ ] Manual verification required |
| 8 | Module registry and module listing | Implemented | Covered by `ModuleRegistryTest.cc` | [ ] Manual verification required |
| 9 | Run orchestration, result ingest, and persistence | Partial; plan identifies unfinished ingest/CRUD stages | Partial; run service tests exist | [ ] Manual verification required |
| 10 | Log retrieval and live SSE tail | Implemented | Unknown; stream behavior needs integration validation | [ ] Manual verification required |
| 11 | OTA upload and trigger workflow | Partial | Unknown | [ ] Manual verification required |
| 12 | Trends, distributions, heatmaps, anomalies, comparisons, and baselines | Implemented | Partial; service tests cover selected behavior | [ ] Manual verification required |
| 13 | Typed threshold rules and anomaly services | Implemented | Covered by `ThresholdServiceTest.cc` and `AnomalyServiceTest.cc` | [ ] Manual verification required |
| 14 | Stream backpressure handling | Implemented | Covered by `StreamBackpressureTest.cc` | [ ] Manual verification required |
| 15 | Docker Compose development environment | Implemented | Unknown | [ ] Manual verification required |
| 16 | Prometheus metrics | Partial; documented as optional | Unknown | [ ] Manual verification required |

## Build and Test

```bash
cmake --preset debug && cmake --build build/debug
cmake --preset test && cmake --build build/test
ctest --test-dir build/test --output-on-failure
```

Docker setup is documented in `docker/README.md`; API details and environment
defaults remain in this repository’s existing sections and `.env.example`.
- A **REST/HTTP API** (`/api/v1`) for the Web UI and external tooling.
- A **PostgreSQL** backend with versioned migrations.
- **OTA firmware upgrade** orchestration.
- **Prometheus metrics** (optional). --- ## 🚀 Quick Start (Docker — Recommended) **The easiest and most reliable way** to get started on any machine: ```bash
cd docker
./setup.sh
``` This will:
1. Check Docker prerequisites
2. Create `.env` from `.env.example`
3. Build the Docker image for your platform (auto-detected)
4. Start PostgreSQL and dev environment
5. You're ready to build! Then enter the container and build:
```bash
docker exec -it joshua-dev bash
cmake --preset debug && cmake --build build/debug
``` ### Alternative: VS Code Dev Container Open this project in VS Code and:
1. Install the "Dev Containers" extension
2. Press `F1` → "Dev Containers: Reopen in Container"
3. Wait for build (first time takes ~10 min) Everything is configured automatically! See [docker/README.md](docker/README.md) for detailed Docker documentation. --- ## Architecture Overview ```
HAL Client (embedded) ──gRPC (port 9090)──► HalServiceImpl │
Web UI/ REST clients ──HTTP (port 8080)──► Drogon REST API │ ┌──────────┴──────────┐ │ Service Layer │ │ RunService │ │ LogService │ │ AuthService │ │ OtaService ... │ └──────────┬──────────┘ │ ┌──────────┴──────────┐ │ Repository Layer │ │ (Drogon ORM/ PG) │ └──────────┬──────────┘ │ PostgreSQL 15+
``` --- ## Prerequisites (Native Build) **Note:** Using Docker (see Quick Start above) avoids all manual dependency installation. For native builds without Docker: | Tool | Version |
|------|---------|
| CMake | 3.22+ |
| vcpkg | latest |
| C++ compiler | GCC 12+ or Clang 16+ |
| PostgreSQL | 15+ |
| protoc + grpc_cpp_plugin | matching vcpkg grpc version | Set `VCPKG_ROOT` in your environment before building. --- ## Quick Start (Local Dev) ```bash
cp .env.example .env
# Edit .env — at minimum set JOSHUA_DB_* and JOSHUA_SESSION_SECRET # Start Postgres
docker compose -f docker/docker-compose.yml up postgres -d # Build and run (loads .env automatically)
./scripts/run_dev.sh
``` Or with Docker Compose (full stack): ```bash
docker compose -f docker/docker-compose.yml up --build
``` --- ## Building ```bash
# Debug (default dev preset)
cmake --preset debug
cmake --build build/debug # Release
cmake --preset release
cmake --build build/release # Run tests
cmake --preset test
cmake --build build/test
ctest --test-dir build/test --output-on-failure # ASan
cmake --preset asan
cmake --build build/asan
``` --- ## Configuration All configuration is via environment variables. See `.env.example` for the full list. | Variable | Default | Description |
|----------|---------|-------------|
| `JOSHUA_GRPC_LISTEN` | `0.0.0.0:9090` | gRPC bind address |
| `JOSHUA_HTTP_LISTEN` | `0.0.0.0:8080` | HTTP bind address |
| `JOSHUA_DB_HOST` | `localhost` | PostgreSQL host |
| `JOSHUA_DB_NAME` | `joshua` | Database name |
| `JOSHUA_DB_USER` | `joshua` | DB username |
| `JOSHUA_DB_PASSWORD` | *(required)* | DB password |
| `JOSHUA_SESSION_SECRET` | *(required, ≥32 chars)* | Session signing key |
| `JOSHUA_REGISTRY_PATH` | `registry.yaml` | Module registry YAML |
| `JOSHUA_OTA_STORAGE_PATH` | `/var/joshua/ota` | OTA artifact directory |
| `JOSHUA_LOG_LEVEL` | `info` | Log level (trace/debug/info/warn/error) | --- ## Module Registry Modules are declared in a YAML file (path set by `JOSHUA_REGISTRY_PATH`): ```yaml
modules: - id: "com.example.temperature" name: "Temperature Monitor" description: "Reads temperature from sensor A" version: "1.2.0" result_type_url: "type.googleapis.com/joshua.TempResult" config_type_url: "type.googleapis.com/joshua.TempConfig" modes: ["single_shot", "monitoring"]
``` --- ## Database Migrations SQL migration files live in `migrations/`. They are applied automatically at startup in alphabetical order. Migrations are idempotent (tracked in `schema_migrations`). To add a new migration, create `migrations/NNNN_description.sql`. --- ## API All protected endpoints require either:
- A session cookie (from `POST/api/v1/auth/login`), or
- An `Authorization: Bearer <token>` header. ### Key endpoints | Method | Path | Description |
|--------|------|-------------|
| GET | `/api/v1/health` | Health check |
| POST | `/api/v1/auth/login` | Login → sets session |
| POST | `/api/v1/auth/tokens` | Create API token |
| GET | `/api/v1/modules` | List test modules |
| POST | `/api/v1/runs` | Start a test run |
| GET | `/api/v1/runs/{id}` | Get run details |
| GET | `/api/v1/runs/{id}/logs` | List run logs |
| GET | `/api/v1/runs/{id}/logs/tail` | SSE live log tail |
| GET | `/api/v1/target` | Get board info |
| POST | `/api/v1/target/heartbeat` | Board heartbeat |
| POST | `/api/v1/ota/upload` | Upload OTA artifact |
| POST | `/api/v1/ota/{id}/trigger` | Trigger OTA upgrade |
| GET | `/api/v1/modules/{id}/trend` | Module metric trend (time-series) |
| GET | `/api/v1/modules/{id}/distribution` | Module metric distribution (histogram) |
| GET | `/api/v1/modules/{id}/heatmap` | Module metric heatmap (test × time cells) |
| GET | `/api/v1/modules/{id}/anomalies` | Z-score anomaly detection over a metric |
| GET | `/api/v1/runs/{a}/compare/{b}` | Two-run side-by-side comparison |
| GET | `/api/v1/runs/{id}/baseline-diff` | Drift/status diff vs the mode's baseline run |
| POST | `/api/v1/runs/{id}/baseline` | Mark a run as the baseline for its mode |
| GET | `/api/v1/thresholds` | List threshold rules (filter by `module_id`/`test_id`) |
| POST | `/api/v1/thresholds` | Create a typed threshold rule |
| DELETE | `/api/v1/thresholds/{id}` | Delete a threshold rule | --- ## Decisions Log | Decision | Rationale |
|----------|-----------|
| gRPC sync API for unary, streaming wait-loop | Simpler than async CQ for initial impl; async CQ adds complexity without benefit at this scale |
| Drogon for REST | Built-in PG ORM, async, WebSocket, SSE, session management |
| argon2id for passwords | OWASP recommended |
| SHA-256 for API tokens | Tokens are long random strings; SHA-256 is fast and sufficient |
| 1-minute minimum cron resolution | Full cron parser adds a dependency; document the limitation |
| Single `board` row (unique partial index) | The system targets exactly one physical test board |
| Migrations at startup | Ensures schema is always up to date; no separate migration tool needed |
| vcpkg manifest mode | Reproducible dependency pinning per repo | ## Build ```sh
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j$(nproc)
``` Binary: `build/joshua-server` ## Run (dev) ```sh
./build/joshua-server --config etc/joshua/server.toml --log-level debug
``` Without a config file the binary starts with compiled-in defaults. ## Test ```sh
cmake --build build --target config-test
ctest --test-dir build --output-on-failure
``` ## Config Default path: `/etc/joshua/server.toml`. See `etc/joshua/server.toml` for all keys and defaults. CLI flags override file values: | Flag | Default |
|------|---------|
| `--config PATH` | `/etc/joshua/server.toml` |
| `--log-level LEVEL` | `info` | ## Implementation Status | Step | Status |
|------|--------|
| 1 · Skeleton + config + logger | ✓ done |
| 2 · DB layer + migrations | — |
| 3 · DB CRUD (runs + batches) | — |
| 4 · Ingest queue + result writer | — |
| 5 · gRPC bidi server | — |
| … | — |
