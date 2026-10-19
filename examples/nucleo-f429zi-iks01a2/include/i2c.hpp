#ifndef I2C_H
#define I2C_H

#include <libecl/communication/i2c/ibus_driver.hpp>

class I2cDriver : public libecl::communication::i2c::IBusDriver
{
public:
	void init();

	bool read(uint8_t device_address, std::span<uint8_t> rx) override;
	bool write(uint8_t device_address, std::span<const uint8_t> tx) override;
	bool writeAndRead(uint8_t device_address, std::span<const uint8_t> tx, std::span<uint8_t> rx) override;
};

#endif