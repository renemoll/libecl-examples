

#include "clock.hpp"

#include <cassert>

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_utils.h>

//! \todo LL API
// uint32_t SystemCoreClock = 168e6;
// const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

static constexpr const uint32_t HseFrequency = 8e6;				   //!\todo Board property
static constexpr const uint32_t HseBypass = LL_UTILS_HSEBYPASS_ON; //!\todo Board property
static constexpr const uint32_t PllOutputFrequency = 168e6;

namespace {
void configureRegulator()
{
	// check PLL is off and HSE/HSI as clock source.

	// configure the voltage regulator
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	auto powerscale = LL_PWR_REGU_VOLTAGE_SCALE3;
	if (PllOutputFrequency >= 144) {
		powerscale = LL_PWR_REGU_VOLTAGE_SCALE1;
	} else if (PllOutputFrequency >= 120) {
		powerscale = LL_PWR_REGU_VOLTAGE_SCALE2;
	}

	LL_PWR_SetRegulVoltageScaling(powerscale);

	// following check only works when the PLL is on..
	// while (LL_PWR_IsActiveFlag_VOS() && LL_PWR_GetRegulVoltageScaling() == powerscale) {
	//!\todo Add a timeout
	// }

	if (PllOutputFrequency >= 180) {
		LL_PWR_EnableOverDriveMode();
		while (LL_PWR_IsEnabledOverDriveMode() == 0) {
			//!\todo Add a timeout
		}

		LL_PWR_EnableOverDriveSwitching();
		while (LL_PWR_IsEnabledOverDriveSwitching() == 0) {
			//!\todo Add a timeout
		}
	}
}

/*!
 *	\details Performs the following actions:
 *				- Enable & configure HSE
 *				- Configure PLL
 *				- Configure FLASH latency
 *				- Configuer bus clocks
 *				- Set SystemCoreClock
 *	\todo Configure Bus clocks
 */
void configurePll()
{
	auto pllConfig = []() {
		LL_UTILS_PLLInitTypeDef config = {};
		config.PLLM = LL_RCC_PLLM_DIV_4;
		config.PLLN = 168;
		config.PLLP = LL_RCC_PLLP_DIV_2;
		return config;
	}();
	auto clkConfig = []() {
		LL_UTILS_ClkInitTypeDef config = {};
		config.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
		config.APB1CLKDivider = LL_RCC_APB1_DIV_4;
		config.APB2CLKDivider = LL_RCC_APB2_DIV_2;
		return config;
	}();

	const auto result = LL_PLL_ConfigSystemClock_HSE(
		HseFrequency,
		HseBypass,
		&pllConfig,
		&clkConfig);

	if (result != SUCCESS) {
		assert("Unable to configure the clock domain");
	}
}

/*!
 *	\todo SysTick is connected to the processor clock, desired?
 *	\todo SysTick interrupt prio?
 *	\todo SystemCoreClock not set/updated after configuring the PLL
 */
void configureSystick()
{
	// const auto ticks = (SystemCoreClock / 1000000U) - 1;
	const auto ticks = (SystemCoreClock / 1000U) - 1; // 1ms for STM32 HAL
	if (!SysTick_Config(ticks)) {
		assert("Unable to configure SysTick");
	}
}

void configureBackupDomain()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_BKPSRAM);
	LL_PWR_EnableBkUpAccess();
}
}

void configureClockDomain()
{
	configureRegulator();
	configurePll();
	configureSystick();
	// configureBackupDomain();
}
