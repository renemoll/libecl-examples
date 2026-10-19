#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# SPDX-FileCopyrightText: 2025-2026 René Moll
# SPDX-License-Identifier: MPL-2.0
#

FROM ubuntu:26.04

ARG CATCH2_VERSION=3.16.1
ARG CLANG_VERSION=22
ARG GCC_VERSION=16
ARG GCC_ARM_VERSION=15.3.rel2
# Note: beware to match the version of include-what-you-use with the clang version
ARG INCLUDE_WHAT_YOU_USE_VERSION=0.26

ARG USER_NAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

LABEL org.opencontainers.image.title="Build environment for libecl"
LABEL org.opencontainers.image.authors="René Moll"
LABEL org.opencontainers.image.licenses="MPL-2.0"

#
# Required tools
#

RUN apt-get update && \
	apt-get install -y --no-install-recommends \
		ca-certificates \
		git \
		gnupg2 \
		python3-minimal \
		python3-pip \
		sudo \
		wget \
		xz-utils && \
	apt-get autoremove -y && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

#
# Install arm-gcc toolchain
#

RUN mkdir /opt/gcc-arm-none-eabi \
 && wget -qO - "https://gitlab.arm.com/api/v4/projects/tooling%2Fgnu-toolchains-for-arm/packages/generic/gnu-toolchain/${GCC_ARM_VERSION}/arm-gnu-toolchain-${GCC_ARM_VERSION}-$(uname -m)-arm-none-eabi.tar.xz" | tar --strip-components=1 -xJC /opt/gcc-arm-none-eabi
ENV PATH="$PATH:/opt/gcc-arm-none-eabi/bin"

#
# Toolchain
#

RUN wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-snapshot-keyring.gpg && \
	apt-get update && \
	apt-get install -y --no-install-recommends \
		# Clang
		clang-${CLANG_VERSION} \
		clang-format-${CLANG_VERSION} \
		clang-tidy-${CLANG_VERSION} \
		clangd-${CLANG_VERSION} \
		libclang-rt-${CLANG_VERSION}-dev \
		lld-${CLANG_VERSION} \
		lldb-${CLANG_VERSION} \
		llvm-${CLANG_VERSION} \
		# CMake
		cmake \
		ninja-build \
		make \
		# GCC
		g++-${GCC_VERSION} \
		gdb \
		# External libraries
		libbenchmark-dev \
		# Code analysis
		cppcheck \
		flawfinder \
		gcovr && \
	pip install --break-system-packages cmakelang cpplint reuse && \
	apt-get autoremove -y && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*
ENV PATH="$PATH:/usr/lib/llvm-${CLANG_VERSION}/bin/"

#
# Include-what-you-use
#

RUN apt-get update && apt-get install -y --no-install-recommends libclang-${CLANG_VERSION}-dev llvm-${CLANG_VERSION}-dev libsqlite3-dev && \
	wget -qO - https://github.com/include-what-you-use/include-what-you-use/archive/refs/tags/${INCLUDE_WHAT_YOU_USE_VERSION}.tar.gz | tar xz -C /tmp && \
	CC=clang CXX=clang++ cmake -S /tmp/include-what-you-use-${INCLUDE_WHAT_YOU_USE_VERSION} -B /tmp/include-what-you-use-${INCLUDE_WHAT_YOU_USE_VERSION}/build && \
	cmake --build /tmp/include-what-you-use-${INCLUDE_WHAT_YOU_USE_VERSION}/build --target install && \
	apt-get purge -y libclang-${CLANG_VERSION}-dev llvm-${CLANG_VERSION}-dev libsqlite3-dev gnupg2 && \
	apt-get autoremove -y && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*

#
# Catch2
#

RUN wget -qO - https://github.com/catchorg/Catch2/archive/refs/tags/v${CATCH2_VERSION}.tar.gz | tar xz -C /tmp && \
	cmake -S /tmp/Catch2-${CATCH2_VERSION} -B /tmp/Catch2-${CATCH2_VERSION}/build && \
	cmake --build /tmp/Catch2-${CATCH2_VERSION}/build --target install

#
# trompeloeil
#

RUN git clone https://github.com/rollbear/trompeloeil.git && \
	cd trompeloeil && \
	cmake -B build \
	  -DTROMPELOEIL_INSTALL_TARGETS=On \
	  -DTROMPELOEIL_BUILD_TESTS=Off \
	  -DCMAKE_BUILD_TYPE=Release \
	  . && \
	cmake --build build --target install && \
	rm -rf trompeloeil

#
# Setup environment
#

RUN update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-${GCC_VERSION} 20 && \
	update-alternatives --install /usr/bin/cc cc /usr/bin/gcc-${GCC_VERSION} 20 && \
	update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-${GCC_VERSION} 20 && \
	update-alternatives --install /usr/bin/python python /usr/bin/python3 10

RUN groupadd --non-unique --gid $USER_GID $USER_NAME \
    && useradd --non-unique --gid $USER_GID --uid $USER_UID --home /home/$USER_NAME $USER_NAME \
    && echo $USER_NAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USER_NAME \
    && chmod 0440 /etc/sudoers.d/$USER_NAME

USER $USER_UID
