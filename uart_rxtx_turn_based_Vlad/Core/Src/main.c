#include "main.h"
#include "stm32f3xx.h"

#include "usart1.h"
#include "uart4.h"
#include "sys_clock.h"
#include "stdbool.h"

#define MAX_LENGTH 64U

char c_in = ' ';
char c_out = ' ';
char current_string[MAX_LENGTH];
uint8_t len = 0U;
uint8_t received_len = 0U;
bool switch_to_receive = false;
char received_string[MAX_LENGTH];

void transmit_strings(void);
void receive_strings(void);

int main(void)
{
	clock_init();
	usart1_init();
	uart4_init();

	while(1)
	{
		if(!switch_to_receive)
			transmit_strings();
		else
			receive_strings();
	}

}
void transmit_strings(void)
{
	usart1_receive_byte(&c_in);

	if((c_in == '\r') || (c_in == '\n'))
	{
		if(len > 0)
		{
			current_string[len++] = '\n';
			current_string[len] = '\0';
			uart4_transmit_string(current_string);
			usart1_transmit_byte('\r');
			usart1_transmit_string("Vlad: ");
			usart1_transmit_string(current_string);
			len = 0U;
			switch_to_receive = true;
			return;
		}

	}
	else
	{
		if(len < (MAX_LENGTH - 1))
		{
			current_string[len++] = c_in;
			usart1_transmit_byte(c_in);
		}
	}
}

void receive_strings(void)
{
	do{
		uart4_receive_byte(&c_out);
		received_string[received_len++] = c_out;
	}while(c_out != '\n');

	received_string[received_len] = '\0';
	usart1_transmit_string("\rCosmin: ");
	usart1_transmit_string(received_string);
	usart1_transmit_byte('\n');

	memset(&received_string[0], 0, sizeof(received_string));
	received_len = 0U;
	switch_to_receive = false;
}

