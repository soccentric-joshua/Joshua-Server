# Joshua-Server Build and Deployment

## Purpose and scope

Joshua-Server is the C++20 server-side service. Its CMake project builds `joshua-server`, generates protobuf/gRPC sources from the sibling `Joshua-HAL/protos` directory, embeds migration paths, and builds unit/integration/config compatibility tests. The root Makefile supports native CMake/Ninja, ARM64 Docker workflows, a separate x86 Ubuntu 22.04 server container, SSH deployment, analysis, and Doxygen.

## 1. Prerequisites

- GNU Make, CMake, Ninja, a C++20 compiler, and `nproc`.
- CMake packages/libraries: gRPC, Protobuf, Drogon, spdlog, yaml-cpp, GTest, OpenSSL, Argon2, and `protoc` plus `grpc_cpp_plugin`. Prometheus-cpp is optional for metrics.
- The sibling `Joshua-HAL` checkout must be present because protocol sources are read from `../Joshua-HAL/protos`.
- Docker with the required platform support for ARM64 and AMD64 workflows. `make docker-install` may install Docker with `sudo apt`.
- For quality/documentation: `clang-format`, `cppcheck`, `doxygen`, and Graphviz as applicable.
- For remote deployment: `sshpass`, SSH/SCP connectivity, compatible target devices, and credentials. Defaults are `nvidia`/`nvidia` and `192.168.1.3`/`192.168.1.13`.

## 2. Setup

Run commands from the repository root. Native configuration is created in `build/native`; server-container configuration is created at `/workspace/Joshua-Server/build/docker` inside the `joshua-dev` image. The Docker workflows mount the parent `Joshua/` directory so sibling repositories remain available.

## 3. Build and compile

- `make configure` - configure native CMake/Ninja with `-DBUILD_TESTS=ON`.
- `make build` - compile native targets.
- `make docker-configure` / `make docker-build` - configure/build the ARM64 cross tree.
- `make docker-image-server` - build the `joshua-dev` x86 server image when absent.
- `make docker-build-server` - configure/build the server in the x86 Ubuntu 22.04 container.
- `make docker-shell` / `make docker-shell-server` - open the corresponding development shell.
- `make list` - list native and cross application outputs.

Native outputs are under `build/native/bin` and `build/native/lib`; the server-container output is under its mounted `build/docker` directory. The primary installed executable is `joshua-server`.

## 4. Test and lint

- `make test` - native CTest suite.
- `make docker-test` - ARM64 CTest through Docker/QEMU.
- `make docker-test-server` - server unit tests in the x86 container.
- `make format` - run the Ninja formatter and CMake formatting.
- `make cppcheck` - run the Ninja cppcheck target.
- `make doc` - generate Doxygen with Ninja target `doc_doxygen`.

The test build includes discovered unit and integration tests plus the `config-test` compatibility test. Analysis targets require their named tools; the Makefile's native path does not provide a cloud or integration environment.

## 5. Packaging and artifacts

The Makefile does not define a CPack/package target for this leaf. Build artifacts are the `joshua-server` executable, libraries, generated protobuf sources, and test binaries in the configured build directories. Deployment should preserve the server's migrations because CMake embeds the source migrations path for runtime lookup.

## 6. Deployment, install, and run

- `make deploy-a` / `make deploy-b` - ARM64-build and copy `build/cross/lib` and `build/cross/bin` to `/tmp/acu-test/` on the chosen target.
- `make deploy` - deploy to both configured devices.
- `make run-a [app] [args...]` / `make run-b [app] [args...]` - deploy and execute a selected remote binary, or all binaries with no app.
- `make run [app] [args...]` - run native applications with `LD_LIBRARY_PATH=build/native/lib`.
- `make docker-build-server` - build for the x86 server container; it does not start a production service.

Remote deployment requires network access, valid credentials, and compatible runtime libraries. The Makefile clears `/tmp/acu-test` before copying and uses `StrictHostKeyChecking=no`; use only on intended lab devices. No cloud deployment or service-manager install target is defined here. A real server run also requires its database, configuration, TLS, migrations, and external service dependencies to be provisioned separately.

## 7. Cleanup

- `make clean` removes native/cross `build/` output.
- `make docker-clean` removes the ARM64 cross build directory.
- `make docker-clean-server` removes the x86 server-container build directory.
- `make setup-completion` appends the completion script to `~/.bashrc`.

## 8. Hardware, cloud, and manual verification caveats

Native and x86 container tests can run without target hardware, while ARM64 Docker tests use emulation. Verify gRPC/protobuf compatibility, database migrations, network binding, TLS, external service dependencies, and device-facing behavior in the intended deployment environment. There is no cloud credential or production rollout automation in this leaf; remote target variables and credentials must be supplied deliberately.