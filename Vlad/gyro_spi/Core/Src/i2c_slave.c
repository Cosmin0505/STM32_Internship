/*
 * i2c_slave.c
 *
 *  Created on: Aug 12, 2026
 *      Author: savav
 */

#include "i2c_slave.h"
#include "usart1.h"
#include "stm32f3xx.h"
#include <string.h>

static const char HEX[] = "0123456789ABCDEF";

static uint8_t g_staging[GYRO_PACKET_LEN];
static uint8_t g_tx_buf[GYRO_PACKET_LEN];
static volatile uint8_t g_tx_idx;
static uint8_t g_rx_buf[16];
static volatile uint8_t g_rx_idx;
static uint8_t g_seq;

volatile uint32_t g_i2c_berr, g_i2c_arlo, g_i2c_ovr;

static char     g_dump_line[64];    /* static: outlives the transfer */
static uint8_t  g_dump_pkt[GYRO_PACKET_LEN];

static uint8_t crc8(const uint8_t *p, uint32_t n)
{
	uint8_t crc = 0x00u;

	while(n--)
	{
		crc ^= *p++;
		for(uint8_t i = 0u; i < 8u; i++){
			crc = (uint8_t)((crc & 0x80u) ? ((crc << 1) ^ 0x07u) : (crc << 1));
		}
	}
	return crc;
}

static void put_le32(uint8_t *dst, int32_t v)
{
	uint32_t u = (uint32_t) v;

	dst[0] = (uint8_t)(u & 0xFFu);
	dst[1] = (uint8_t)((u >> 8) & 0xFFu);
	dst[2] = (uint8_t)((u >> 16) & 0xFFu);
	dst[3] = (uint8_t)((u >> 24) & 0xFFu);
}

void i2c1_gpio_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	(void)RCC->AHBENR;

	/* Configure pins PB8 and PB9 */
	GPIOB->MODER &= ((3U << (8 * 2)) | (3U << (9 * 2)));
	GPIOB->MODER |= ((2U << (8 * 2)) | (2U << (9 * 2)));

	GPIOB->OTYPER |= (1U << 8) | (1U << 9);

	GPIOB->OSPEEDR |= (3U << (8 * 2)) | (3U << (9 * 2));

	GPIOB->PUPDR &= ~((3U << (8 * 2)) | (3U << (9 * 2)));

	GPIOB->AFR[1] &= ~((0xFU << (8 - 8) * 4) | (0xFU << (9 - 8) * 4));
	GPIOB->AFR[1] |= (4U << (8 - 8) * 4) | (4U << (9 - 8) * 4);
}

void i2c_slave_init(void)
{
	i2c1_gpio_init();

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
	NVIC_SetPriority(I2C1_ER_IRQn, 1U);
	NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void i2c_slave_publish(int32_t x, int32_t y, int32_t z, uint8_t status)
{
	NVIC_DisableIRQ(I2C1_EV_IRQn);
	__DSB();
	__ISB();

	g_staging[0] = 0xA5u;
	g_staging[1] = ++g_seq;
	g_staging[2] = status;
	put_le32(&g_staging[3], x);
	put_le32(&g_staging[7], y);
	put_le32(&g_staging[11], z);
	g_staging[15] = crc8(g_staging, 15u);

	NVIC_EnableIRQ(I2C1_EV_IRQn);
}

static void i2c_take_snapshot(void)
{
	memcpy(g_tx_buf, g_staging, GYRO_PACKET_LEN);
	g_tx_idx = 0u;
}

static void i2c_handle_command(void)
{
	if(g_rx_idx >= 1){
		switch(g_rx_buf[0]){
		case 0x01u: default: break;
		}
	}
	g_rx_idx = 0u;
}

void I2C1_EV_IRQHandler(void)
{
	uint32_t isr = I2C1->ISR;

	if(isr & I2C_ISR_ADDR)
	{
		if(isr & I2C_ISR_DIR)
		{
			I2C1->ISR |= I2C_ISR_TXE;

			if(g_rx_idx > 0u) {
				i2c_handle_command();
			}

			i2c_take_snapshot();
		}
		else
		{
			g_rx_idx = 0u;
		}

		I2C1->ICR = I2C_ICR_ADDRCF;
		isr = I2C1->ISR;
	}

	if(isr & I2C_ISR_RXNE)
	{
		uint8_t b = (uint8_t)I2C1->RXDR;
		if(g_rx_idx < sizeof g_rx_buf){
			g_rx_buf[g_rx_idx++] = b;
		}
	}

	if(isr & I2C_ISR_TXIS){
		uint8_t b = 0xFFu;
		if(g_tx_idx < GYRO_PACKET_LEN)
		{
			b = g_tx_buf[g_tx_idx++];
		}
		I2C1->TXDR = b;
	}

	if(isr & I2C_ISR_NACKF)
	{
		I2C1->ICR = I2C_ICR_NACKCF;
	}

	if(isr & I2C_ISR_STOPF)
	{
		I2C1->ICR = I2C_ICR_STOPCF;
		if(g_rx_idx > 0u)
		{
			i2c_handle_command();
		}
	}
}

void I2C1_ER_IRQHandler(void)
{
	uint32_t isr = I2C1->ISR;

	if(isr & I2C_ISR_BERR)
	{
		I2C1->ICR = I2C_ICR_BERRCF;
		g_i2c_berr++;
	}
	if(isr & I2C_ISR_ARLO)
	{
		I2C1->ICR = I2C_ICR_ARLOCF;
		g_i2c_arlo++;
	}
	if(isr & I2C_ISR_OVR)
	{
		I2C1->ICR = I2C_ICR_OVRCF;
		g_i2c_ovr++;
	}
}

void i2c_slave_get_packet(uint8_t *dst)
{
	memcpy(dst, g_staging, GYRO_PACKET_LEN);
}

static void hex_line(const uint8_t *p, uint32_t len, char *out)
{
    uint32_t o = 0u;
    for (uint32_t i = 0u; i < len; i++) {
        out[o++] = HEX[(p[i] >> 4) & 0x0Fu];
        out[o++] = HEX[ p[i]       & 0x0Fu];
        out[o++] = ' ';
    }
    out[o++] = '\r';
    out[o++] = '\n';
    out[o]   = '\0';
}

void dump_packet_hex(void)
{
    /* USART1_TX is normally DMA1 Channel 4 on the F303 — adjust to match
     * your dma1_init(). Skip the dump if the last one is still going.  */
    if ((DMA1_Channel4->CCR & DMA_CCR_EN) && (DMA1_Channel4->CNDTR != 0u)) {
        return;
    }

    i2c_slave_get_packet(g_dump_pkt);
    hex_line(g_dump_pkt, GYRO_PACKET_LEN, g_dump_line);
    usart1_write(g_dump_line);
}
