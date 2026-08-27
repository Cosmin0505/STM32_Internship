#ifndef INC_STM32F3XX_H_
#define INC_STM32F3XX_H_

#include "stm32f3xx.h"
#include <stdint.h>

/* LCD pins defines */
#define LCD_ADDRESS 0x3F
#define LCD_RS (1U << 0)
#define LCD_RW (1U << 1)
#define LCD_E (1U << 2)
#define LCD_BT (1U << 3)
#define LCD_DATA (1U << 4)

/* I2C1 slave mode defines */
#define I2C_SLAVE_ADDR_7BIT 0x42u
#define GYRO_PACKET_LEN 12u

/* L3GD20 Gyroscope defines */
#define L3GD20_READ 0x80u
#define L3GD20_WRITE 0x00u
#define L3GD20_AUTOINC 0x40u
#define L3GD20_WHO_AM_I 0x0Fu
#define L3GD20_CTRL_REG1 0x20u
#define L3GD20_CTRL_REG4 0x23u
#define L3GD20_CTRL_REG5 0x24u
#define L3GD20_STATUS_REG 0x27u
#define L3GD20_OUT_X_L 0x28u
#define L3GD20_STATUS_ZYXDA (1U << 3)

/* L3GD20 Gyroscope calibration defines */
#define CAL_DISCARD_SAMPLES    20u
#define CAL_SAMPLE_COUNT      128u
#define CAL_MAX_SPREAD_LSB     300
#define CAL_MAX_BIAS_LSB      1400

/* Used for storing measured bias during calibration of the gyroscope */
typedef struct {
    int16_t x, y, z;
    uint8_t valid;
} gyro_bias_t;

/*
 * @brief: This function blocks the execution of the program for a specified amount of time.
 * @param: uint32_t ms - the amount of time the delay is applied for
 * @retval: none
*/
void delay_ms(int ms);

/*
 * @brief: This function transforms 4 bytes of data from Big-Endian into Little-Endian
 * @param: uint8_t *dst - pointer to the array where the converted data will be stored
 * 		   int32_t v - data to be transformed
 * @retval: none
 *  */
void put_le32(uint8_t *dst, int32_t v);

/*
 * @brief: This function:
 * 		   - enables GPIOA clock
 * 		   - sets pin PA0 as analog input
 * 		   - calibrates and configures ADC1 CH1 for conversion
 * @param: none
 * @retval: none
*/
void adc1_init(void);

/*
 * @brief: This function initiates a conversion on ADC1
 * @param: none
 * @retval: none
*/
void adc1_start_conversion(void);

/*
 * @brief: This function:
 * 		   - waits until the conversion is complete
 * 		   - returns the converted value from the DR register of ADC1
 * @param: none
 * @retval: uint32_t - the converted value read in the DR register of ADC1
 *
*/
uint32_t adc1_read(void);

/*
 * @brief: This function:
 * 		   - configures the LED in 4-bit mode and 2 lines
 * 		   - automatically increments the cursor after each character
 * @param: none
 * @retval: none
*/
void lcd_init(void);

/*
 * @brief: This function:
 * 		   - assembles the nibble of data and the flags into one message
 * 		   - splits the messsage into two nibbles (high and low)
 * 		   - sequentially sends the two nibbles to the LCD display
 * 		   - pulses the EN line
 *
 * @param: uint8_t nibble - the nibble (actual data) to send to the LCD screen
 * 		   uint8_t flags - the flags that should accompany the message
 * @retval: none
*/
void lcd_write_nibble(uint8_t nibble, uint8_t flags);

/*
 * @brief: This function:
 * 		   - splits the command (one byte) into two nibbles (high and low)
 * 		   - sequentially sends them to the LCD display
 * 		   - RS bit reset signals to the LCD display that the message is supposed to be a command
 * @param: uint8_t cmd - command byte to be sent to the LCD display
 * @retval: none
*/
void lcd_sendcommand(uint8_t cmd);

/*
 * @brief: This function:
 * 		   - splits the data (one byte) into two nibbles (high and low)
 * 		   - sequentially sends them to the LCD display
 * 		   - RS bit set signals to the LCD display that the message is supposed to be displayed on the screen
 * @param: uint8_t data - data byte to be sent to the LCD display
 * @retval: none
*/
void lcd_senddata(uint8_t data);

/*
 * @brief: This function takes a sequence of chars and individually sends them to be displayed on the LCD display
 * @param: char *s - pointer to the first element of a char sequence
 * @retval: none
*/
void lcd_write(char *s);

/*
 * @brief: This function:
 * 		   - configures PB8 as SCL for I2C1
 * 		   - configures PB9 as SDA for I2C1
 * 		   - configures I2C1 in master mode @ 100kHz
 * @param: none
 * @retval: none
 *  */
void i2c1_master_init(void);

/*
 * @brief: This function:
 * 		   - configures PB8 as SCL for I2C1
 * 		   - configures PB9 as SDA for I2C1
 * 		   - configures I2C1 in slave mode
 * 		   - configures its own address at 0x42
 * @param: none
 * @retval: none
 * */
void i2c1_slave_init(void);

/*
 * @brief: This function writes a data buffer to a slave using its 7-bit address
 * @param: uint8_t addr - 7-bit address of the slave
 * 		   uint8_t *data - pointer to the data buffer to be sent
 * 		   uint32_t length - length of data buffer
 * @retval: none
 *  */
void i2c1_write(uint8_t addr, uint8_t *data, uint32_t length);

/*
 * @brief: This functions stores three variables into a staging buffer in Little-Endian format to be transmitted over I2C
 * 		   when requested by the master
 * @param: int32_x - first variable (in this case contains the acceleration on the x-axis)
 * 		   int32_y - second variable (in this case contains the acceleration on the y-axis)
 * 		   int32_z - third variable (in this case contains the acceleration on the x-axis)
 * 		   uint8_t *g_staging - buffer where the data will be stored
 * @retval: none
 * */
void i2c1_slave_publish(int32_t x, int32_t y, int32_t z, uint8_t *g_staging);

/*
 * @brief: This function:
 * 		   - configures pin PA6 as TIM3_CH1 output
 * 		   - configures TIM3 in PWM mode
 * 		   - configures PWM frequency to be 1 kHz
 * @param: none
 * @retval: none
 *  */
void tim3_pwm_init();

/*
 * @brief: This function updates the duty cycle of the generated PWM signal
 * @param: uint32_t duty - the compare value (0 - 999) that determines the duty cycle
 * @retval: none
 *  */
void tim3_pwm_update_duty_cycle(uint32_t duty);

/*
 * @brief: This function:
 * 		   - configures pin PA5, PA6, PA7 for SPI1 communication
 * 		   - configures SPI1 in master mode, 8-bit and fclk/64 baud rate communication
 * @param: none
 * @retval: none
 *  */
void spi1_init(void);

/*
 * @brief: This function transmits one byte over SPI1 and simultaneously returns the received byte
 * @param: uint8_t tx - the byte to be transmitted
 * @retval: uint8_t - the byte received
 *  */
uint8_t spi1_transfer(uint8_t tx);

/*
 * @brief: This function waits for SPI1 to finish and clears unread received data
 * @param: none
 * @retval: none
 *  */
void spi1_wait_idle(void);

/* @brief: This function writes data to the gyroscope's registers; used for configuring the gyroscope
 * @param: uint8_t reg - the address of the register to write into
 * 		   uint8_t value - the byte of data to be written inside the register
 * @retval: none
*/
void gyro_write_reg(uint8_t reg, uint8_t value);

/*
 * @brief: This function configures the gyroscope and allows it to stabilize
 * @param: none
 * @retval: none
 * */
void gyro_init(void);

/*
 * @brief: This function blocks execution until new data is available for all three axes
 * @param: none
 * @retval: none
 * */
void gyro_wait_data_ready(void);

/*
 * @brief: This function calibrates the gyroscope by calculating its biases
 * @param: gyro_bias_t *g_bias - stores the calculated biases
 * @retval: 1 if operation was successful, 0 otherwise (if the device was moved during the calibration process)
 *  */
uint8_t gyro_calibrate(gyro_bias_t *g_bias);

/*
 * @brief: This function reads consecutive gyroscope registers using SPI auto-increment feature.
 * 		   Used for obtaining the gyroscope readings for every axis.
 * @param: uint8_t start_reg - address of the first register to be read
 * 		   uint8_t *buf - address of the buffer where the data will be stored
 * 		   uint8_t len - length of read data
 * @retval: none
 * */
void gyro_read_multi(uint8_t start_reg, uint8_t *buf, uint8_t len);

/*
 * @brief: This function reads the raw (without bias correction) x/y/z gyroscope values
 * @param: int16_t *x - pointer to the variable where the x-axis reading will be stored
 * 		   int16_t *y - pointer to the variable where the y-axis reading will be stored
 * 		   int16_t *z - pointer to the variable where the z-axis reading will be stored
 * @retval: none
 *  */
void gyro_read_xyz(int16_t *x, int16_t *y, int16_t *z);

/*
 * @brief: This function reads the bias-corrected readings in mdps
 * @param: volatile int32_t *x - pointer to the variable where the bias-corrected x-axis reading will be stored
 * 		   volatile int32_t *y - pointer to the variable where the bias-corrected y-axis reading will be stored
 * 		   volatile int32_t *z - pointer to the variable where the bias-corrected z-axis reading will be stored
 * @retval: none
 *  */
void gyro_read_xyz_mdps(volatile int32_t *x, volatile int32_t *y, volatile int32_t *z);


#endif
