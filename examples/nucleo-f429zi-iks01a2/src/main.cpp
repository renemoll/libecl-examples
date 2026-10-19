
#include "clock.hpp"
#include "heartbeat.hpp"
#include "i2c.hpp"

#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include "stm32f4xx_hal.h"

// extern "C" {
// void SystemInit(void)
// {
// #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
//     SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
// #endif

// #if defined(USER_VECT_TAB_ADDRESS)
//     SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET;
// #endif
// }
// }


void initGpio()
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

	LL_GPIO_InitTypeDef init = {};
	LL_GPIO_StructInit(&init);
	init.Pin = LL_GPIO_PIN_7;
	init.Mode = LL_GPIO_MODE_OUTPUT;
	LL_GPIO_Init(GPIOB, &init);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
}

Heartbeat g_heartBeat(500000);

int main() {
    /**
     * TODO: configure MCU
     * - prefetch
     */
    
    //  __HAL_RCC_SYSCFG_CLK_ENABLE();
    configureClockDomain();
    SystemCoreClockUpdate();
   
    initGpio();

    I2cDriver i2c;
    i2c.init();

    std::array<const uint8_t, 1> data = {0x0F};
    i2c.write(data);

    std::array<uint8_t, 2> rx_data = {0x00};
    i2c.read(rx_data);

    while(true)
    {
        if (g_heartBeat.periodElapsed()) {
			LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_7);
		}
    }

    return 0;
}


extern "C" {
void SysTick_Handler()
{
    static uint32_t count = 0;
    ++count;
    g_heartBeat.update(count);

    HAL_IncTick();
}
}
