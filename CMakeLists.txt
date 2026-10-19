#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# SPDX-FileCopyrightText: 2026 René Moll
# SPDX-License-Identifier: MPL-2.0
#

cmake_minimum_required(VERSION 3.23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/bob-cmake")

project(libecl-examples-stm32
	LANGUAGES
		# ASM C CXX
		CXX
	VERSION
		0.1.0
)

if(NOT PROJECT_IS_TOP_LEVEL)
	message(FATAL_ERROR "This project is meant to be the top-level project, not to be included in another build.")
endif()

set(EXAMPLE_DEFAULT_OPTION ${PROJECT_IS_TOP_LEVEL})
option(EXAMPLE_BUILD_TESTS "Build tests" ${EXAMPLE_DEFAULT_OPTION})

#
# Project options.
#


#
# C++ standard configuration.
# Allow for C++ standard to be set via CLI.
#

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

#
# Include cmake utilities, bob.
#

include(bob)

# Include the external dependencies
add_subdirectory(external/libecl)

# #
# # CMSIS_5
# #

# add_library(cmsis INTERFACE)
# target_include_directories(cmsis
# 	INTERFACE
# 		"${CMAKE_CURRENT_LIST_DIR}/external/CMSIS_5/CMSIS/Core/Include"
# )

# add_library(stm32-hal
# 	OBJECT
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_cortex.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_dma.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_i2c.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_i2c_ex.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_gpio.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_ll_gpio.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Src/stm32f4xx_ll_utils.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/cmsis-device-f4/Source/Templates/system_stm32f4xx.c"
# 	"${CMAKE_CURRENT_LIST_DIR}/external/cmsis-device-f4/Source/Templates/gcc/startup_stm32f429xx.s"
# )
# target_include_directories(stm32-hal
# 	PUBLIC
# 		"${CMAKE_CURRENT_LIST_DIR}/external/cmsis-device-f4/Include"
# 		"${CMAKE_CURRENT_LIST_DIR}/external/stm32f4xx-hal-driver/Inc"
# 		"${CMAKE_CURRENT_LIST_DIR}/examples/nucleo-f429zi-iks01a2/include"
# )
# target_compile_definitions(stm32-hal
# 	PUBLIC
# 		USE_FULL_LL_DRIVER
# 		STM32F429xx
# 		HSE_VALUE=8000000
# )
# target_link_libraries(stm32-hal
# 	PUBLIC
# 		cmsis
# )
# target_compile_options(stm32-hal
# 	PRIVATE
# 		-Wno-pedantic
# )

# add_subdirectory(examples)


#
# Add project code.
#

add_subdirectory(app)

if(EXAMPLE_BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
	enable_testing()
	add_subdirectory(test)
endif()
