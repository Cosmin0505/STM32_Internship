#include "main.h"
#include "stm32f3xx.h"

#include "usart1.h"
#include "uart4.h"
#include "gyro.h"
#include "i2c_slave.h"
#include "sys_clock.h"
#include <stdbool.h>

char c_in_uart4 = ' ';
char c_in_uart1 = ' ';

char pc_received_string[MAX_LENGTH];
char string_to_be_transmitted[MAX_LENGTH] = "Vlad: ";
char coords[MAX_LENGTH];

char received_string_uart4[MAX_LENGTH];
char string_to_be_transmitted_uart4[MAX_LENGTH] = "Cosmin: ";

static uint32_t dump_div;

static volatile uint8_t id;
volatile uint8_t  g_cal_ok;
volatile int32_t  g_bias_mdps_x, g_bias_mdps_y, g_bias_mdps_z;
volatile int32_t  g_mdps_x, g_mdps_y, g_mdps_z;

gyro_bias_t g_bias = { 0, 0, 0, 0 };



int main(void)
{
	clock_init();
	usart1_init();
	uart4_init();
	dma1_init();
	dma2_init();
	spi1_gpio_init();
	spi1_init();

	id = gyro_read_reg(L3GD20_WHO_AM_I);
	if(id != 0xD4u && id != 0xD3u){
		while(1)
		{
			__NOP();
		}
	}

	gyro_init();

	do {
		g_cal_ok = gyro_calibrate();
	} while (!g_cal_ok);

	g_bias_mdps_x = raw_to_mdps(g_bias.x);
	g_bias_mdps_y = raw_to_mdps(g_bias.y);
	g_bias_mdps_z = raw_to_mdps(g_bias.z);

	i2c_slave_publish(0, 0, 0, GYRO_ST_CAL_VALID);

	i2c_slave_init();

	while(1)
	{
		gyro_wait_data_ready();
		gyro_read_xyz_mdps(&g_mdps_x, &g_mdps_y, &g_mdps_z);

		i2c_slave_publish(g_mdps_x, g_mdps_y, g_mdps_z, GYRO_ST_CAL_VALID);

	    if (++dump_div >= 19u) {
	        dump_div = 0u;
	        dump_packet_hex();
	    }
	}

}



