#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# SPDX-FileCopyrightText: 2026 René Moll
# SPDX-License-Identifier: MPL-2.0
#

# add_executable()
add_library(example
    "${CMAKE_CURRENT_LIST_DIR}/src/configuration.cpp"
)

target_sources(example
    PUBLIC
        FILE_SET
			example_headers
        TYPE
            HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_LIST_DIR}/include"
        FILES
            "${CMAKE_CURRENT_LIST_DIR}/include/example/configuration.hpp"
        
)

set_target_properties(example
	PROPERTIES
		LINKER_LANGUAGE CXX
)

target_compile_features(example
	PUBLIC
		cxx_std_20
)