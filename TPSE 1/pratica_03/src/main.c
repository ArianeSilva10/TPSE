/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2024.1, Agosto 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Francisca Ariane
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */
//setenv app "setenv serverip 10.10.0.1; setenv ipaddr  10.10.0.2; tftp 0x80000000 appGpio.bin; go 0x80000000"
//run app
#include "hw_types.h"

#define TIME                                                    1000000
#define TIME2                                                   100000
#define TOGGLE          										(0x01u) // Alterna o estado do  led



#include	"gpio.h"
#include 	"interruption.h"
#include 	"timers.h"
//#include 	"uart.h"
#define DELAY_USE_INTERRUPT			1


/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
void ledON(gpioMod, ucPinNumber);
void ledOFF(gpioMod,  ucPinNumber);
int button_press(gpioMod, ucPinNumber);
void delay_LOW();
void delay_RUN();
/****************************************************************************
**				GLOBAL VARIABLES
****************************************************************************/
unsigned int flagBlink;

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

void delay_LOW(){
	unsigned int ra;
	for ( ra = 0; ra < TIME; ra++);
}

void delay_RUN(){
	unsigned int ra;
	for (ra = 0; ra < TIME2; ra++);
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */

void ISR_Handler(void){
	/*interrupt handling*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */

int main(void)
{
	/*-----------------------------------------------------------------------------
	 *  Desable WatchDog
	 *-----------------------------------------------------------------------------*/
	disableWdt();
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO modules
	 *-----------------------------------------------------------------------------*/
	
	unsigned int led = 29, button = 15, button2 = 12, interno1 = 21, interno2 = 22, 
	interno3 = 23, interno4 = 24;

	/*-----------------------------------------------------------------------------
	 *  Configure GPIO pin and multiplexing
	 *-----------------------------------------------------------------------------*/
	Init_module_gpio(GPIO1); // modulo que usarei
	
	Init_pin_gpio(GPIO1,button,INPUT); // Inicializo o pino do botão como entrada

	Init_pin_gpio(GPIO1,led,OUTPUT); // inicializo como saida (led)
	Init_pin_gpio(GPIO1,interno1,OUTPUT); // inicializo como saida (led)
	Init_pin_gpio(GPIO1,interno2,OUTPUT); // inicializo como saida (led)
	Init_pin_gpio(GPIO1,interno3,OUTPUT); // inicializo como saida (led)
	Init_pin_gpio(GPIO1, interno4,OUTPUT); // inicializo como saida (led)

	Init_pin_gpio(GPIO1, button2, INPUT);

	Set_direction_pin_gpio(GPIO1, led, OUTPUT);
	Set_direction_pin_gpio(GPIO1, interno1, OUTPUT);
	Set_direction_pin_gpio(GPIO1, interno2, OUTPUT);
	Set_direction_pin_gpio(GPIO1, interno3, OUTPUT);
	Set_direction_pin_gpio(GPIO1, interno4, OUTPUT);


	Set_direction_pin_gpio(GPIO1, button, INPUT);
	Set_direction_pin_gpio(GPIO1, button2, INPUT);

	ledON(GPIO1, led);
	delay_LOW();
	ledOFF(GPIO1, led);
	delay_LOW();

	while (1)
	{
		ledON(GPIO1, led);
		delay_RUN();

		ledOFF(GPIO1, led);
		delay_RUN(); 

		if (button_press(GPIO1, button))
		{
			ledON(GPIO1, led);
			delay_RUN();

			ledOFF(GPIO1, led);
			delay_RUN();

			ledON(GPIO1, interno1);
			ledON(GPIO1, interno2);
			//ledON(GPIO1, interno3);
			ledON(GPIO1, interno4);
			delay_RUN();

			ledOFF(GPIO1, interno1);
			ledOFF(GPIO1, interno2);
			ledOFF(GPIO1, interno4);
			delay_RUN();
		}else if (button_press(GPIO1, button2))
		{
			ledON(GPIO1, interno1);
			ledON(GPIO1, interno2);
			ledON(GPIO1, interno3);
			//ledON(GPIO1, interno4);
			delay_RUN();

			ledOFF(GPIO1, interno1);
			ledOFF(GPIO1, interno2);
			ledOFF(GPIO1, interno3);
			delay_RUN();
		}
		
	}
	

	return 0;

} // end of function main

void ledON(gpioMod mod, ucPinNumber pino){
	GpioSetPinValue(mod, pino, HIGH); // em nivel alto, chama a função que acende o led
	// se o parametro level for LOW entao apaga o led
}

void ledOFF(gpioMod mod, ucPinNumber pino){
	GpioSetPinValue(mod, pino, LOW);
}

int button_press(gpioMod mod, ucPinNumber pino){
	return GpioGetPinValue(mod, pino); // retorna HIGH ou LOW
	// para verificar se o botão foi pressionado, se seu bit estiver setado(1)
	// entao ele retorna seu estado
}
