
add_executable(nucleo-f429zi-iks01a2)

target_sources(nucleo-f429zi-iks01a2
	PRIVATE
		src/clock.cpp
		src/heartbeat.cpp
		src/i2c.cpp
		src/main.cpp
)

target_include_directories(nucleo-f429zi-iks01a2
	PRIVATE
		"${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(nucleo-f429zi-iks01a2
	PRIVATE
		libecl::libecl
		stm32-hal
)

bob_firmware_image(nucleo-f429zi-iks01a2 "${CMAKE_CURRENT_LIST_DIR}/STM32F429XX_FLASH.ld")
