#
# SPDX-FileCopyrightText: 2026 René Moll
# SPDX-License-Identifier: CC0-1.0
#

.PHONY: clang-coverage
clang-coverage:
	cmake --workflow --preset coverage-clang-debug

.PHONY: clang-unittest
clang-unittest:
	cmake --workflow --preset unittest-clang-debug

.PHONY: clang-unittest-release
clang-unittest-release:
	cmake --workflow --preset unittest-clang-release

.PHONY: gcc-coverage
gcc-coverage:
	cmake --workflow --preset coverage-gcc-debug

.PHONY: gcc-unittest
gcc-unittest:
	cmake --workflow --preset unittest-gcc-debug

.PHONY: gcc-unittest-release
gcc-unittest-release:
	cmake --workflow --preset unittest-gcc-release

.PHONY: gcc-m7
gcc-m7:
	cmake --workflow --preset build-gcc-cortex-m7-debug
