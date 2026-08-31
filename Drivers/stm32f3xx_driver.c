#include "stm32f3xx_driver.h"

void clock_init(void)
{
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;

	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY));

	RCC->CR &= ~RCC_CR_PLLON;
	while(RCC->CR & RCC_CR_PLLRDY);

	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL |
					RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PLLMUL12
				| RCC_CFGR_HPRE_DIV1
				| RCC_CFGR_PPRE1_DIV2
				| RCC_CFGR_PPRE2_DIV1);

	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;

}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

void put_le32(uint8_t *dst, int32_t v)
{
	uint32_t u = (uint32_t) v;

	dst[0] = (uint8_t)(u & 0xFFu);
	dst[1] = (uint8_t)((u >> 8) & 0xFFu);
	dst[2] = (uint8_t)((u >> 16) & 0xFFu);
	dst[3] = (uint8_t)((u >> 24) & 0xFFu);
}

void adc1_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~(3U << (0 * 2));
	GPIOA->MODER |= (3U << (0 * 2));

	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	/* Use AHB_clk / 2 as the ADC clock */
    ADC1_2_COMMON->CCR &= ~(3U << 16);
    ADC1_2_COMMON->CCR |= (2U << 16);

    /* Enable ADC1 internal voltage regulator */
	ADC1->CR &= ~(ADC_CR_ADVREGEN_1 | ADC_CR_ADVREGEN_0);
	ADC1->CR |= ADC_CR_ADVREGEN_0;


	/* Allow voltage regulator to stabilize */
	for(int i = 0; i < 1000; i++)
	{

	}

	/* Select ADC1 CH1 as first regular conversion */
	ADC1->SQR1 |= 1U << 6;


	/* Enable ADC1 and wait until it's ready */
	ADC1->CR |= ADC_CR_ADEN;

	while(!(ADC1->ISR & ADC_ISR_ADRDY))
	{

	}
}


void adc1_start_conversion(void)
{
	ADC1->CR |= ADC_CR_ADSTART;
}


uint32_t adc1_read(void)
{
	/* Wait until conversion is complete */
	while(!(ADC1->ISR & ADC_ISR_EOS))
	{

	}

	return (uint32_t)ADC1->DR;
}


void lcd_init(void)
{
	/* Allow LCD to power on */
	delay_ms(50);

	/* Initialize in 4-bit mode */
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	lcd_write_nibble(0x20u, 0);

	lcd_sendcommand(0x28); // 4-bit mode and 2 lines
	lcd_sendcommand(0x0C); // Display on, cursor off
	lcd_sendcommand(0x06); // Increment cursor after each character
	lcd_sendcommand(0x01); // Clear display
	delay_ms(2);
}


void lcd_write_nibble(uint8_t nibble, uint8_t flags)
{
	uint8_t value = ((nibble & 0xF0u) | flags | LCD_BT);

	uint8_t data_high = value | LCD_E;
	i2c1_write(LCD_ADDRESS, &data_high, 1);
	delay_ms(1);

	uint8_t data_low = value & ~LCD_E;
	i2c1_write(LCD_ADDRESS, &data_low, 1);
	delay_ms(1);
}


void lcd_sendcommand(uint8_t cmd)
{
	lcd_write_nibble((cmd & 0xF0u), 0);
	lcd_write_nibble(((cmd << 4) & 0xF0u), 0);
}


void lcd_senddata(uint8_t data)
{
	lcd_write_nibble((data & 0xF0u), LCD_RS);
	lcd_write_nibble(((data << 4) & 0xF0u), LCD_RS);
}


void lcd_write(char *s)
{
	while(*s)
	{
		lcd_senddata(*s++);
	}
}

void i2c1_master_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	(void)RCC->AHBENR;

	GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOB->MODER |= ((2U << (8 * 2)) | (2U << (9 * 2)));

	GPIOB->OTYPER |= (1U << 8) | (1U << 9);

	GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

	GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	/* Select AF4 for both pins */
	GPIOB->AFR[1] &= ~((0xFU << (8 - 8) * 4) | (0xFU << (9 - 8) * 4));
	GPIOB->AFR[1] |= (4U << (8 - 8) * 4) | (4U << (9 - 8) * 4);

	/* Select the 8 MHz HSI clock as I2C1 clock source */
	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;

	/* Enable I2C1 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC->APB1ENR; // Wait for initialization to complete

	/* Disable I2C1 before configuration */
	I2C1->CR1 &= ~I2C_CR1_PE;
	while(I2C1->CR1 & I2C_CR1_PE){ }

	/* 100khz @ fI2Cclk 8MHz */
	I2C1->TIMINGR = 0x10420F13U;
	I2C1->CR2 = 0U;

	/* Clear pending status flags */
	I2C1->ICR |= I2C_ICR_STOPCF
			   | I2C_ICR_NACKCF
			   | I2C_ICR_BERRCF
			   | I2C_ICR_ARLOCF
			   | I2C_ICR_OVRCF;

	/* Enable interrupt sources */
	I2C1->CR1 |= I2C_CR1_TXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	/* Enable I2C1 */
	I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_slave_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	(void)RCC->AHBENR;

	/* Configure pins PB8 and PB9 */
	GPIOB->MODER &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOB->MODER |= ((2U << (8 * 2)) | (2U << (9 * 2)));

	GPIOB->OTYPER |= (1U << 8) | (1U << 9);

	GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

	GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	GPIOB->AFR[1] &= ~((0xFU << (8 - 8) * 4) | (0xFU << (9 - 8) * 4));
	GPIOB->AFR[1] |= (4U << (8 - 8) * 4) | (4U << (9 - 8) * 4);

	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	(void)RCC->APB1ENR; // wait for initialization complete

	I2C1->CR1 &= ~I2C_CR1_PE;
	while(I2C1->CR1 & I2C_CR1_PE){ }

	/* 100khz @ fI2Cclk 8MHz */
	I2C1->TIMINGR = 0x10420F13U;

	I2C1->OAR1 = 0U;
	I2C1->OAR1 = I2C_OAR1_OA1EN | ((uint32_t) I2C_SLAVE_ADDR_7BIT << 1);
	I2C1->OAR2 = 0U;
	I2C1->CR2 = 0U;

	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

	I2C1->CR1 |= I2C_CR1_ADDRIE
			   | I2C_CR1_RXIE
			   | I2C_CR1_TXIE
			   | I2C_CR1_STOPIE
			   | I2C_CR1_NACKIE
			   | I2C_CR1_ERRIE;

	I2C1->CR1 |= I2C_CR1_PE;

	NVIC_SetPriority(I2C1_EV_IRQn, 1U);
	NVIC_EnableIRQ(I2C1_EV_IRQn);
}

void i2c1_write(uint8_t addr, uint8_t *data, uint32_t length)
{
	/* Wait if I2C bus is busy */
	while(I2C1->ISR & I2C_ISR_BUSY)
	{

	}

	/* Clear previous transfer status flags */
	I2C1->ICR = I2C_ICR_STOPCF |
			    I2C_ICR_NACKCF |
				I2C_ICR_BERRCF |
				I2C_ICR_ARLOCF |
				I2C_ICR_OVRCF;

	/* Set the address, byte count and automatic stop signal and start transfer */
	I2C1->CR2 = ((uint32_t) addr << 1U) |
				(length << 16U) |
				I2C_CR2_AUTOEND |
				I2C_CR2_START;

	/* Send each byte from buffer when transmit register is available */
	for(int i = 0; i < length; i++)
	{
		while(!(I2C1->ISR & I2C_ISR_TXIS))
		{
		}
		I2C1->TXDR = data[i];
	}

	/* Wait until automatic stop signal is transmitted */
	while(!(I2C1->ISR & I2C_ISR_STOPF))
	{
	}

	I2C1->ICR = I2C_ICR_STOPCF;
}

void i2c1_slave_publish(int32_t x, int32_t y, int32_t z, uint8_t *g_staging)
{

	// Check if interrupts were enabled before
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	/* Convert x/y/z values into little-endian and store them inside staging buffer  */
	put_le32(&g_staging[0], x);
	put_le32(&g_staging[4], y);
	put_le32(&g_staging[8], z);

	__DMB();

	//If interrupts were enabled before, re-enable them
	if(primask == 0U)
	{
		__enable_irq();
	}
}

void i2c_take_snapshot(uint8_t *g_tx_buf, uint8_t *g_staging)
{
	memcpy(g_tx_buf, g_staging, GYRO_PACKET_LEN);
	g_tx_index = 0u;
}

void tim3_pwm_init()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	GPIOA->MODER &= ~(3U << (6 * 2));
	GPIOA->MODER |= (2U << (6 * 2));

	/* Select AF2 (TIM3_CH1) for PA6 */
	GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
	GPIOA->AFR[0] |= (2U << (6 * 4));

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Timer tick frequency is ftim3 / (TIM3_PSC + 1) => 48 / 48 MHz = 1 MHz */
	TIM3->PSC = 47;
	/* PWM frequency is ftim3 / ((PSC + 1) * (ARR + 1)) */
	TIM3->ARR = 999;

	/* Select PWM mode for TIM3_CH1 */
	TIM3->CCMR1 &= ~(0xFU << 4);
	TIM3->CCMR1 |= (6U << 4);

	/* Enable pre-load to ensure safe duty cycle updates */
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

	/* Enable CH1 output and TIM3 peripheral */
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void tim3_pwm_update_duty_cycle(uint32_t duty)
{
	TIM3->CCR1 = duty;
}

void spi1_init(void)
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
	/* Wait until the transmit register can receive data */
	while((SPI1->SR & SPI_SR_TXE) == 0U);

	/* Store the byte to send inside the data register */
	*(volatile uint8_t *)&SPI1->DR = tx;

	/* Wait until received byte is available */
	while((SPI1->SR & SPI_SR_RXNE) == 0U);

	/* Return the received byte */
	return *(volatile uint8_t *)&SPI1->DR;
}

void spi1_wait_idle(void)
{
	/* Wait for transmit register to be empty */
	while((SPI1->SR & SPI_SR_FTLVL) != 0U);

	/* Wait until SPI1 is no longer busy */
	while((SPI1->SR & SPI_SR_BSY) != 0U);

	/* Clear any unread data from the receive register */
	while((SPI1->SR & SPI_SR_FRLVL) != 0U)
	{
		(void)*(volatile uint8_t *)&SPI1->DR;
	}
}

void gyro_init(void)
{
	/* Enable normal operation and measurement for all 3 axes */
	gyro_write_reg(L3GD20_CTRL_REG1, 0x0Fu);

	gyro_write_reg(L3GD20_CTRL_REG4, 0x80u);

	/* Enable low-pass filter */
	gyro_write_reg(L3GD20_CTRL_REG5, 0x02u);

	/* Wait for the gyroscope's output to stabilize */
	delay_ms(250);
}

void gyro_wait_data_ready(void)
{
	/* Poll the status register until all three axes' data-ready flags are set */
	while((gyro_read_reg(L3GD20_STATUS_REG) & L3GD20_STATUS_ZYXDA) == 0U);
}

uint8_t gyro_calibrate(gyro_bias_t *g_bias)
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

int32_t raw_to_mdps(int32_t raw)
{
    /* +/-250 dps sensitivity is 8.75 mdps/LSB = 35/4 exactly.
     * Worst case |raw| = 32768 -> 1 146 880, well inside int32_t. */
    return ((int32_t)raw * 35) / 4;
}

void gyro_read_multi(uint8_t start_reg, uint8_t *buf, uint8_t len)
{
	/* Select the gyroscope by driving CS low */
	GPIOE->BSRR = CS_RESET;

	/* Send the starting address, read request and auto-increment function */
	(void)spi1_transfer((start_reg & 0x3Fu) | L3GD20_READ | L3GD20_AUTOINC);

	/* Store the requested register bytes */
	for(int i = 0; i < len; i++)
	{
		buf[i] = spi1_transfer(0xFFu);
	}

	/* Wait until transfer is complete before driving CS high */
	spi1_wait_idle();
	GPIOE->BSRR = CS_SET;
}

void gyro_read_xyz(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t b[6];

	/* Read the bytes for all three axes in one transfer */
	gyro_read_multi(L3GD20_OUT_X_L, b, 6);

	/* Combine each little-endian byte pair into a signed value */
	*x = (int16_t)(((uint16_t)b[1] << 8) | (uint16_t)b[0]);
	*y = (int16_t)(((uint16_t)b[3] << 8) | (uint16_t)b[2]);
	*z = (int16_t)(((uint16_t)b[5] << 8) | (uint16_t)b[4]);
}

void gyro_read_xyz_mdps(volatile int32_t *x, volatile int32_t *y, volatile int32_t *z)
{
    int16_t rx, ry, rz;

    /* Read the raw samples for each axis */
    gyro_read_xyz(&rx, &ry, &rz);

    /* Subtract the measured bias and convert each raw sample to mdps */
    *x = raw_to_mdps((int32_t)rx - g_bias.x);
    *y = raw_to_mdps((int32_t)ry - g_bias.y);
    *z = raw_to_mdps((int32_t)rz - g_bias.z);
}

void uart4_interrupt_init(uint16_t baud_rate)
{
	/* Enable GPIOC clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Enable UART4 clock */
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

	/* Configure Pins PC10 and PC11 in Alternate function mode */
	GPIOC->MODER &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->MODER |= ((2U << (10 * 2)) | (2U << (11 * 2)));


	/* Configure alternate functions for GPIO pins*/
	GPIOC->AFR[1] &= ~((15U << ((10 - 8) * 4)) | (15U << ((11 - 8) * 4)));
	GPIOC->AFR[1] |= ((5U << ((10 - 8) * 4)) | (5U << ((11 - 8) * 4)));


	/* Configure pins to High Speed */
	GPIOC->OSPEEDR &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->OSPEEDR |= ((3U << (10 * 2)) | (3U << (11 * 2)));

	/* Configure pins output type */
	GPIOC->OTYPER &= ~((1U << 10) | (1U << 11));

	GPIOC->PUPDR &= ~((3U << (10 * 2)) | (3U << (11 * 2)));
	GPIOC->PUPDR |= (1U << (11 * 2));

	/* Configure oversampling of 16 and disable UART during set-up*/
	UART4->CR1 = 0;

	/* Configure the other config registers with default settings */
	UART4->CR2 = 0;
	UART4->CR3= 0;

	/* Configure baud rate */
	UART4->BRR = (APB1CLK + baud_rate / 2) / baud_rate;

	/* Generate an interrupt every time something is received */
	UART4->CR1 |= USART_CR1_RXNEIE;

	/* Enable UART, transmission and reception */
	UART4->CR1 |= UART4_UE | UART4_RE | UART4_TE;

	while((UART4->ISR & USART_ISR_TEACK) == 0U);
	while((UART4->ISR & USART_ISR_REACK) == 0U);

	NVIC_EnableIRQ(UART4_IRQn);
}

void dma2_init(char *string_to_be_transmitted)
{
	/* Enable clock for DMA2 */
	RCC->AHBENR |= DMA2EN;

	/* Disable Channel 5 */
	DMA2_Channel5->CCR &= ~DMA_CCR_EN;

	/* Clear all error flags for Channel 5 */
	DMA2->IFCR |= DMA_IFCR_CGIF5;
	DMA2->IFCR |= DMA_IFCR_CTCIF5;
	DMA2->IFCR |= DMA_IFCR_CHTIF5;
	DMA2->IFCR |= DMA_IFCR_CTEIF5;

	/* Set destination buffer */
	DMA2_Channel5->CPAR = (uint32_t)&UART4->TDR;

	/* Set source buffer */
	DMA2_Channel5->CMAR = (uint32_t)&string_to_be_transmitted_uart4;

	/* Enable memory increment */
	DMA2_Channel5->CCR |= CCR5MINC;

	/* Set memory to peripheral direction */
	DMA2_Channel5->CCR |= CCR5DIR;

	/* Set PSIZE = 8 bit*/
	DMA2_Channel5->CCR &= ~(3U << 8);

	/* Set MSIZE = 8 bit*/
	DMA2_Channel5->CCR &= ~(3U << 10);

	/* Enable DMA transmitter in USART1 */
	UART4->CR3 |= USART_CR3_DMAT;
}

void uart4_transmit_byte(char c)
{
	if((UART4->ISR & USART_ISR_TC))
	{
		UART4->TDR = c;
	}
}

void uart4_receive_byte(char *c){

	if(UART4->ISR & USART_ISR_ORE)
	{
		UART4->ICR |= USART_ICR_ORECF;
	}

	*c = (char)(UART4->RDR);
}

void uart4_transmit_string(char *s)
{
	while(s != '\0')
	{
		uart4_transmit_byte(s);
		s++;
	}
}

bool uart4_tx_busy(void)
{
	return (DMA2_Channel5->CNDTR != 0U);
}

bool uart4_write(const char *s, char *tx_buf)
{
	if(uart4_tx_busy())
		return false;

	uint16_t len = strlen(s);

	if(len == 0U)
		return false;

	memcpy(tx_buf, s, len);



	DMA2_Channel5->CCR &= ~DMA_CCR_EN;

	DMA2->IFCR |= DMA_IFCR_CGIF5;
	DMA2->IFCR |= DMA_IFCR_CTCIF5;
	DMA2->IFCR |= DMA_IFCR_CHTIF5;
	DMA2->IFCR |= DMA_IFCR_CTEIF5;

	DMA2_Channel5->CMAR = (uint32_t)tx_buf;
	DMA2_Channel5->CNDTR = len;
	DMA2_Channel5->CCR |= DMA_CCR_EN;

	return true;
}

void usart1_interrupt_init(uint16_t baud_rate)
{
	/* Enable GPIOC clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	/* Enable USART1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	/* Configure Pins PC4 and PC5 in Alternate function mode */
	GPIOC->MODER &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->MODER |= ((2U << (4 * 2)) | (2U << (5 * 2)));


	/* Configure alternate functions for GPIO pins*/
	GPIOC->AFR[0] &= ~((15U << (4 * 4)) | (15U << (5 * 4)));
	GPIOC->AFR[0] |= ((7U << (4 * 4)) | (7U << (5 * 4)));


	/* Configure pins to High Speed */
	GPIOC->OSPEEDR &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->OSPEEDR |= ((3U << (4 * 2)) | (3U << (5 * 2)));

	/* Configure pins output type */
	GPIOC->OTYPER &= ~(1U << 4 | 1U << 5);

	GPIOC->PUPDR &= ~((3U << (4 * 2)) | (3U << (5 * 2)));
	GPIOC->PUPDR |= (1U << (5 * 2));

	/* Configure oversampling of 16 and disable UART during set-up*/
	USART1->CR1 = 0;

	/* Configure the other config registers with default settings */
	USART1->CR2 = 0;
	USART1->CR3= 0;


	/* Configure baud rate */
	USART1->BRR = (APB2CLK + BAUD_RATE / 2) / BAUD_RATE;


	/* Generate an interrupt every time something is received */
	USART1->CR1 |= USART_CR1_RXNEIE;

	/* Enable UART, transmission and reception */
	USART1->CR1 |= USART1_UE | USART1_RE | USART1_TE;

	while((USART1->ISR & USART_ISR_TEACK) == 0U);
	while((USART1->ISR & USART_ISR_REACK) == 0U);

	NVIC_SetPriority(USART1_IRQn, 2);
	NVIC_EnableIRQ(USART1_IRQn);
}

void dma1_init(char *string_to_be_transmitted)
{
	/* Enable clock for DMA1 */
	RCC->AHBENR |= DMA1EN;

	/* Disable Channel 4 */
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;

	/* Clear all error flags for Channel 4 */
	DMA1->IFCR |= DMA_IFCR_CGIF4;
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
	DMA1->IFCR |= DMA_IFCR_CHTIF4;
	DMA1->IFCR |= DMA_IFCR_CTEIF4;

	/* Set destination buffer */
	DMA1_Channel4->CPAR = (uint32_t)&USART1->TDR;

	/* Set source buffer */
	DMA1_Channel4->CMAR = (uint32_t)&string_to_be_transmitted;

	/* Enable memory increment */
	DMA1_Channel4->CCR |= CCR4MINC;

	/* Set memory to peripheral direction */
	DMA1_Channel4->CCR |= CCR4DIR;

	/* Set PSIZE = 8 bit*/
	DMA1_Channel4->CCR &= ~(3U << 8);

	/* Set MSIZE = 8 bit*/
	DMA1_Channel4->CCR &= ~(3U << 10);

	/* Enable DMA transmitter in USART1 */
	USART1->CR3 |= USART_CR3_DMAT;
}

void usart1_transmit_byte(char *c)
{
	if((USART1->ISR & USART_ISR_TC))
	{
		USART1->TDR = c;
	}
}

void usart1_receive_byte(char *c)
{
	if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
	{
		*c = (char)(USART1->RDR);
	}
}

void usart1_transmit_string(char *s)
{
	while(s != '\0')
	{
		usart1_transmit_byte(s);
		s++;
	}
}

bool usart1_tx_busy(void)
{
	return (DMA1_Channel4->CNDTR != 0U);
}

bool usart1_write(const char *s, char *tx_buf)
{
	if(usart1_tx_busy())
		return false;

	uint16_t len = strlen(s);

	if(len == 0U)
		return false;

	memcpy(tx_buf, s, len);



	DMA1_Channel4->CCR &= ~DMA_CCR_EN;

	DMA1->IFCR |= DMA_IFCR_CGIF4;
	DMA1->IFCR |= DMA_IFCR_CTCIF4;
	DMA1->IFCR |= DMA_IFCR_CHTIF4;
	DMA1->IFCR |= DMA_IFCR_CTEIF4;

	DMA1_Channel4->CMAR = (uint32_t)tx_buf;
	DMA1_Channel4->CNDTR = len;
	DMA1_Channel4->CCR |= DMA_CCR_EN;

	return true;
}
