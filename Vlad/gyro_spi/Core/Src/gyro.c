/*
 * gyro.c
 *
 *  Created on: Aug 10, 2026
 *      Author: savav
 */

#include "stm32f3xx.h"
#include "gyro.h"

extern gyro_bias_t g_bias;

void gyro_delay_ms(uint32_t ms)
{
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    while (ms--) {
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0U) { }
    }

    SysTick->CTRL = 0U;
}

void spi1_gpio_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;

	/* PA5, PA6, PA7 pins*/
	GPIOA->MODER &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));
	GPIOA->MODER |= ((2U << (5 * 2)) | (2U << (6 * 2)) | (2U << (7 * 2)));

	GPIOA->OTYPER &= ~((1U << 5) | (1U << 6) | (1U << 7));

	/* High speed */
	GPIOA->OSPEEDR |= ((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));

	/* No pull-up */
	GPIOA->PUPDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)) | (3U << (7 * 2)));

	/* Configure alternate function no. 5 */
	GPIOA->AFR[0] &= ~(((0xFU) << (5 * 4)) | (0xFU << (6 * 4)) | (0xFU << (7 * 4)));
	GPIOA->AFR[0] |= (((5U) << (5 * 4)) | (5U << (6 * 4)) | (5U << (7 * 4)));

	/* Disable PE3 (chip select pin)*/
	GPIOE->BSRR = (1U << 3);

	GPIOE->MODER &= ~(3U << (3 * 2));
	GPIOE->MODER |= (1U << (3 * 2));
	GPIOE->OTYPER &= ~(1U << 3);
	GPIOE->OSPEEDR |= (3U << (3 * 2));
	GPIOE->PUPDR &= ~(3U << (3 * 2));

	GPIOE->BSRR = (1U << 3);
}

void spi1_init(void)
{
	/* Enable SPI1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	SPI1->CR1 = 0;
	SPI1->CR2 = 0;

	/* Set master mode */
	SPI1->CR1 |= SPI_CR1_MSTR;
	/* Clock idles at high logical level, and data is sampled on trailing clock edge  */
	SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;
	/* Configure baud rate to fclk / 64*/
	SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;

	/* Configure data length to 8 bits*/
	SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	SPI1->CR2 |= SPI_CR2_FRXTH;

	/* Enable SPI after configuration */
	SPI1->CR1 |= SPI_CR1_SPE;
}

uint8_t spi1_transfer(uint8_t tx)
{
	while((SPI1->SR & SPI_SR_TXE) == 0U);

	*(volatile uint8_t *)&SPI1->DR = tx;

	while((SPI1->SR & SPI_SR_RXNE) == 0U);

	return *(volatile uint8_t *)&SPI1->DR;
}

void spi1_wait_idle(void)
{
	while((SPI1->SR & SPI_SR_FTLVL) != 0U);
	while((SPI1->SR & SPI_SR_BSY) != 0U);
	while((SPI1->SR & SPI_SR_FRLVL) != 0U)
	{
		(void)*(volatile uint8_t *)&SPI1->DR;
	}
}

uint8_t gyro_read_reg(uint8_t reg)
{
	uint8_t value;
	GPIOE->BSRR = CS_RESET;
	(void)spi1_transfer((reg & 0x3FU) | L3GD20_READ);
	value = spi1_transfer(0xFFu);
	spi1_wait_idle();
	GPIOE->BSRR = CS_SET;
	return value;

}

void gyro_write_reg(uint8_t reg, uint8_t value)
{
	GPIOE->BSRR = CS_RESET;
	(void)spi1_transfer((reg & 0x3Fu) | L3GD20_WRITE);
	(void)spi1_transfer(value);
	spi1_wait_idle();
	GPIOE->BSRR = CS_SET;
}

void gyro_init(void)
{
	gyro_write_reg(L3GD20_CTRL_REG1, 0x0Fu);
	gyro_write_reg(L3GD20_CTRL_REG4, 0x80u);
	gyro_write_reg(L3GD20_CTRL_REG5, 0x02u); // low-pass filter enable

	gyro_delay_ms(250);
}

void gyro_wait_data_ready(void)
{
	while((gyro_read_reg(L3GD20_STATUS_REG) & L3GD20_STATUS_ZYXDA) == 0U);
}



/* Returns 1 on success, 0 if the board was not held still enough.
 * The board MUST be stationary for the whole call (~1.5 s).             */
uint8_t gyro_calibrate(void)
{
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;
    int16_t min_x = INT16_MAX, min_y = INT16_MAX, min_z = INT16_MAX;
    int16_t max_x = INT16_MIN, max_y = INT16_MIN, max_z = INT16_MIN;
    int16_t x, y, z;
    int32_t mean_x, mean_y, mean_z;

    g_bias.valid = 0;

    /* --- 1. Throw away the filter's convergence transient ------------- */
    for (uint32_t i = 0; i < CAL_DISCARD_SAMPLES; i++) {
        gyro_wait_data_ready();
        gyro_read_xyz(&x, &y, &z);
    }

    /* --- 2. Accumulate in raw LSB, tracking min/max per axis ---------- */
    for (uint32_t i = 0; i < CAL_SAMPLE_COUNT; i++) {
        gyro_wait_data_ready();
        gyro_read_xyz(&x, &y, &z);

        sum_x += x;  sum_y += y;  sum_z += z;

        if (x < min_x) { min_x = x; }
        if (x > max_x) { max_x = x; }
        if (y < min_y) { min_y = y; }
        if (y > max_y) { max_y = y; }
        if (z < min_z) { min_z = z; }
        if (z > max_z) { max_z = z; }
    }

    /* --- 3. Reject if the board moved during the run ------------------ */
    if (((int32_t)max_x - min_x) > CAL_MAX_SPREAD_LSB ||
        ((int32_t)max_y - min_y) > CAL_MAX_SPREAD_LSB ||
        ((int32_t)max_z - min_z) > CAL_MAX_SPREAD_LSB) {
        return 0;   /* vibration or a bump — retry */
    }

    mean_x = sum_x / (int32_t)CAL_SAMPLE_COUNT;
    mean_y = sum_y / (int32_t)CAL_SAMPLE_COUNT;
    mean_z = sum_z / (int32_t)CAL_SAMPLE_COUNT;

    /* --- 4. Reject an implausibly large offset ------------------------ */
    /* The spread check above cannot see a CONSTANT rotation: spinning at a
     * steady rate looks perfectly still to a min/max test, and the bias
     * would silently absorb the rotation. A magnitude check catches it. */
    if (mean_x >  CAL_MAX_BIAS_LSB || mean_x < -CAL_MAX_BIAS_LSB ||
        mean_y >  CAL_MAX_BIAS_LSB || mean_y < -CAL_MAX_BIAS_LSB ||
        mean_z >  CAL_MAX_BIAS_LSB || mean_z < -CAL_MAX_BIAS_LSB) {
        return 0;
    }

    g_bias.x = (int16_t)mean_x;
    g_bias.y = (int16_t)mean_y;
    g_bias.z = (int16_t)mean_z;
    g_bias.valid = 1;
    return 1;
}

void gyro_read_multi(uint8_t start_reg, uint8_t *buf, uint8_t len)
{
	GPIOE->BSRR = CS_RESET;
	(void)spi1_transfer((start_reg & 0x3Fu) | L3GD20_READ | L3GD20_AUTOINC);
	for(int i = 0; i < len; i++)
	{
		buf[i] = spi1_transfer(0xFFu);
	}
	spi1_wait_idle();
	GPIOE->BSRR = CS_SET;
}

void gyro_read_xyz_mdps(volatile int32_t *x, volatile int32_t *y, volatile int32_t *z)
{
    int16_t rx, ry, rz;
    gyro_read_xyz(&rx, &ry, &rz);

    *x = raw_to_mdps((int32_t)rx - g_bias.x);
    *y = raw_to_mdps((int32_t)ry - g_bias.y);
    *z = raw_to_mdps((int32_t)rz - g_bias.z);
}


void gyro_read_xyz(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t b[6];
	gyro_read_multi(L3GD20_OUT_X_L, b, 6);

	*x = (int16_t)(((uint16_t)b[1] << 8) | (uint16_t)b[0]);
	*y = (int16_t)(((uint16_t)b[3] << 8) | (uint16_t)b[2]);
	*z = (int16_t)(((uint16_t)b[5] << 8) | (uint16_t)b[4]);
}

int32_t raw_to_mdps(int32_t raw)
{
    /* +/-250 dps sensitivity is 8.75 mdps/LSB = 35/4 exactly.
     * Worst case |raw| = 32768 -> 1 146 880, well inside int32_t. */
    return ((int32_t)raw * 35) / 4;
}
