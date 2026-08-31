/*
 * ********************************************************************
 * @file		stm32f07xx.h
 * @author		Cosmin
 * @brief		Collection of applications and initializations
 * 				for peripherals used for STM32F070RB Nucleo Board.
 *
 * ********************************************************************
 */


#ifndef INC_STM32F07XX_H_
#define INC_STM32F07XX_H_



/*
 * Defines and function prototypes for UART peripheral & Turn-based messenger application
 * between two microcontrollers
 */
#define	UART2_EN		(1U << 17)		/* Bit position of EN (enable UART2) */
#define	UART2_TE		(1U << 3) 		/* Bit position of TE (transmission enable) */
#define UART2_RE		(1U << 2) 		/* Bit position of TE (receive enable) */
#define UART2_UE		(1U << 0) 		/* Bit position of UE (receive enable) */
#define UART2_RXNEIE	(1U << 5) 		/* Bit position of RXNEIE (Interrupt enabled on receive) */
#define	UART2_RXNE		UART2_RXNEIE	/* Bit to check if receive is complete */
#define UART2_TC		(1U << 6)		/* Bit position for TC (transmission complete) flag */
#define UART2_TCCF		UART2_TC		/* Bit position to clear flag of complete transmission */
#define	UART1_EN		(1U << 14)		/* Bit position of EN (enable UART1) */
#define	UART1_TE		UART2_TE 		/* Bit position of TE (transmission enable) */
#define UART1_RE		UART2_RE 		/* Bit position of TE (receive enable) */
#define UART1_UE		UART2_UE 		/* Bit position of UE (receive enable) */
#define UART1_RXNEIE	UART2_RXNEIE	/* Bit position of RXNEIE (Interrupt enabled on receive) */
#define	UART1_RXNE		UART2_RXNE		/* Bit to check if receive is complete */
#define UART1_TC		UART2_TC		/* Bit position for TC (transmission complete) flag */
#define UART1_TCIE		UART2_TC		/* Bit position for TCIE (Transmission complete interrupt enable) */
#define UART1_TCCF		UART2_TCCF		/* Bit position to clear flag of complete transmission */

#define	BAUDRATE		9600			/* Proposed baudrate */
#define FCLK			8000000			/* Clock frequency of Nucleo board */
#define GPIOA_EN		UART2_EN		/* GPIOA enable bit position */

#define MAX_SIZE	100					/* Maximum buffer for messages */
#define CHAR_SIZE	2					/* Send single character as array */

/*
 * @brief  Initialize UART1 peripheral (used for nucleo - discovery)
 * @param  none
 * @retval none
 */
void uart1_init(void);

/*
 * @brief		Read character from UART1
 * @param[in]	char* output - bit to be received
 * @retval		none
 */
void uart1_read(char *output);

/*
 * @brief		Send character to UART1
 * @param[in]	char bit - bit to be transmitted
 * @retval		none
 */
void uart1_write(char bit);

/*
 * @brief		Initialize UART2 peripheral (used for laptop - nucleo)
 * @param		none
 * @retval		none
 */
void uart2_init(void);

/*
 * @brief		Read character from UART2
 * @param[in]	char* output - bit to be received
 * @retval		none
 */
void uart2_read(char *output);

/*
 * @brief		Send character to UART2
 * @param[in]	char bit - bit to be transmitted
 * @retval		none
 */
void uart2_write(char bit);

/*
 * @brief		Send character to UART2
 * @param[in]	char* received_msg - char array containing message to be send
 * @retval		none
 */
void uart2_write_string(char* received_msg);

/*
 * @brief		Transmit function to send messages to Discovery from nucleo
 * @param		none
 * @retval		none
 */
void transmit(void);

/*
 * @brief		Transmission debug function
 * @param[in]	char* send_msg - char array containing message to be send
 * @retval		none
 */
void transmit_debug(char *send_msg);

/*
 * @brief		Receive function to receive messages to Nucleo from Discovery
 * @param[in]	char* received_char - received character
 * @param[in]	char* received_msg - built received message
 * @retval		int8_t flag to switch from read to write
 */
int8_t receive(char *received_char, char *received_msg);



/*
 * Defines and function prototypes for I2C peripheral
 */
#define DISCOVERY_I2C_ADDR_7BIT	(0x42U)		/* Address of Discovery */
#define GYRO_XYZ_LEN			12U			/* Length of buffer of received values */
#define BTN_PIN					(1U << 13)	/* User button pin */
#define	LED_PIN					(1U << 5)	/* Led pin */

/*
 * @brief		GPIO user LED initialization
 * @param		none
 * @retval		none
 */
void gpio_led_init(void);

/*
 * @brief		GPIO User button initialization
 * @param		none
 * @retval		none
 */
void gpio_button_init(void);

/*
 * @brief		EXTI initialization
 * @param		none
 * @retval		none
 */
void exti_init(void);

/*
 * @brief		I2C2 initialization
 * @param		none
 * @retval		none
 */
void i2c2_init(void);

/*
 * @brief		I2C2 initialization in master mode
 * @param		none
 * @retval		none
 */
void gpio_i2c2_init_master(void);





#endif
