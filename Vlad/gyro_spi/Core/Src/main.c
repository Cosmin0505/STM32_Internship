#include "stm32f3xx.h"
#include "gyro.h"
#include "i2c_slave.h"
#include "sys_clock.h"
#include <stdbool.h>


static volatile uint8_t id;
volatile uint8_t  g_cal_ok;
volatile int32_t  g_bias_mdps_x, g_bias_mdps_y, g_bias_mdps_z;
volatile int32_t  g_mdps_x, g_mdps_y, g_mdps_z;

gyro_bias_t g_bias = { 0, 0, 0, 0 };

int main(void)
{
	clock_init();
	spi1_gpio_init();
	spi1_init();

	/* Validate gyroscope device ID */
	id = gyro_read_reg(L3GD20_WHO_AM_I);
	if((id != 0xD4u) && (id != 0xD3u)){
		while(1)
		{
			__NOP();
		}
	}

	gyro_init();

	/* Calibrate gyroscope and store its biases */
	do {
		g_cal_ok = gyro_calibrate();
	} while (!g_cal_ok);

	g_bias_mdps_x = raw_to_mdps(g_bias.x);
	g_bias_mdps_y = raw_to_mdps(g_bias.y);
	g_bias_mdps_z = raw_to_mdps(g_bias.z);

	i2c_slave_publish(0, 0, 0);
	i2c_slave_init();

	while(1)
	{
		gyro_wait_data_ready();
		gyro_read_xyz_mdps(&g_mdps_x, &g_mdps_y, &g_mdps_z);
		i2c_slave_publish(g_mdps_x, g_mdps_y, g_mdps_z);
	}

}



