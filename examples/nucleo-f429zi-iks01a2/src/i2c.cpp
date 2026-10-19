
#include "i2c.hpp"

#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_rcc.h"

static I2C_HandleTypeDef hi2c1;
static DMA_HandleTypeDef hdma_i2c1_rx;
static DMA_HandleTypeDef hdma_i2c1_tx;

namespace{
void configureIO(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void configurePeripheral(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        // Error_Handler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        // Error_Handler();
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        // Error_Handler();
    }

    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}
}

void I2cDriver::init(){
    configureIO();
    configurePeripheral();
}

/**
 * Chip         Device address  | WHO_AM_I
 * ---------
 * LSM6DSL      0xD6            | 0x0F -> 0x6A 
 * LSM303AGR    
 * - ACC        0x32            | 0x0F -> 0x33
 * - MAG        0x3C            |
 * LPS22HB      0xBA            | 0x0F -> 0xB1
 * HTS221       0x5F            | 0x0F -> 0xBC
 */

bool I2cDriver::read(std::span<uint8_t> rx) {
    uint16_t deviceAddress = 0xD6;
    return HAL_I2C_Master_Receive(&hi2c1, deviceAddress, rx.data(), rx.size(), 1000) == HAL_OK;
}

bool I2cDriver::write(std::span<const uint8_t> tx) {
    uint16_t deviceAddress = 0xD6;
    return HAL_I2C_Master_Transmit(&hi2c1, deviceAddress, const_cast<uint8_t*>(tx.data()), tx.size(), 1000) == HAL_OK;
}

bool I2cDriver::readAndWrite(std::span<const uint8_t> tx, std::span<uint8_t> rx) {
    return false;
}

extern "C" {
    
/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_EV_IRQn 0 */

  /* USER CODE END I2C1_EV_IRQn 0 */
  HAL_I2C_EV_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_EV_IRQn 1 */

  /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  /* USER CODE BEGIN I2C1_ER_IRQn 0 */

  /* USER CODE END I2C1_ER_IRQn 0 */
  HAL_I2C_ER_IRQHandler(&hi2c1);
  /* USER CODE BEGIN I2C1_ER_IRQn 1 */

  /* USER CODE END I2C1_ER_IRQn 1 */
}
}