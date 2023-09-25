FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak
COPY ./mirror-sources.list /etc/apt/sources.list

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.27.5"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
    chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
RUN su vscode -c "${VCPKG_ROOT}/vcpkg install grpc"

RUN su vscode -c "${VCPKG_ROOT}/vcpkg install sqlite-orm"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends build-essential gdb

RUN apt update && apt install lsb-release software-properties-common gnupg -y -qq

RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

RUN mv /usr/bin/clang /usr/bin/clang-14
RUN ln -s /usr/bin/clang-17 /usr/bin/clang

RUN mv /usr/bin/clang++ /usr/bin/clang++-14
RUN ln -s /usr/bin/clang++-17 /usr/bin/clang++

RUN ln -s /usr/bin/lld-17 /usr/bin/lld

RUN su vscode -c "${VCPKG_ROOT}/vcpkg install spdlog"