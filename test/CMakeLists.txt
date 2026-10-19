#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# SPDX-FileCopyrightText: 2026 René Moll
# SPDX-License-Identifier: MPL-2.0
#

find_package(Catch2 3 REQUIRED)
include(Catch)

add_executable(unittests)

target_sources(unittests
	PRIVATE
		test_configuration.cpp
)

target_link_libraries(unittests
	PRIVATE
		example
		Catch2::Catch2WithMain
)

catch_discover_tests(unittests)
bob_configure_target(unittests
    # TODO: either use scan-build or ensure system headers use -isystem to avoid warnings
	ENABLE_CLANG_TIDY Off
)