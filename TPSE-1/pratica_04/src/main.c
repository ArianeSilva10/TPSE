/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21/08/2024
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




#include	"gpio.h"
#include 	"timers.h"
#include 	"uart.h"
//#define DELAY_USE_INTERRUPT			1

typedef enum _state{
	SEQ1=1,
	SEQ2,
	SEQ3,
	SEQ4,
	SEQ5,
	SEQ6,
}state;

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
void ledON(gpioMod, ucPinNumber);
void ledOFF(gpioMod,  ucPinNumber);
int button_press(gpioMod, ucPinNumber);
/****************************************************************************
**				GLOBAL VARIABLES
****************************************************************************/


void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}
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
	putString(UART0, "GPIO INPUT INICIALIZADO\n", 24);
	unsigned int op=SEQ1, count = 0;
	ucPinNumber pino=21;
	/*-----------------------------------------------------------------------------
	 *  Desable WatchDog
	 *-----------------------------------------------------------------------------*/
	disableWdt();

	Init_module_gpio(GPIO1);
	Init_module_gpio(GPIO2);

	uartInitModule(UART0, 115200, STOP1, PARITY_NONE, FLOW_OFF);
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO modules
	 *-----------------------------------------------------------------------------*/
	// Configura os pinos 21 a 24 como saídas para controlar os LEDs
		Init_pin_gpio(GPIO1, pino, OUTPUT);
		Init_pin_gpio(GPIO1, pino+1, OUTPUT);
		Init_pin_gpio(GPIO1, pino+2, OUTPUT);
		Init_pin_gpio(GPIO1, pino+3, OUTPUT);
		Init_pin_gpio(GPIO1, pino+4, OUTPUT);

		//delay(1000, TIMER2); // Pequeno delay entre a configuração de cada pino
	// Configura o pino 13 como saída para controlar um LED extra
	Init_pin_gpio(GPIO2, 1, OUTPUT);

	// Configura os pinos 12 como entradas para os botões
	Init_pin_gpio(GPIO1, 16, INPUT);

	// Configura a direção dos pinos 21 a 24 como saídas
		Set_direction_pin_gpio(GPIO1, OUTPUT, pino);
		Set_direction_pin_gpio(GPIO1, OUTPUT, pino+1);
		Set_direction_pin_gpio(GPIO1, OUTPUT, pino+2);
		Set_direction_pin_gpio(GPIO1, OUTPUT, pino+3);
		Set_direction_pin_gpio(GPIO1, OUTPUT, pino+4);


		//delay(1000, TIMER2);// Pequeno delay entre a configuração de cada pino

	// Configura a direção do pino 28 como saída e do pino 12 como entrada
	Set_direction_pin_gpio(GPIO2,OUTPUT, 1);
	Set_direction_pin_gpio(GPIO1, INPUT, 16);

    // Desliga todos os LEDs inicialmente
    /*for(count=pino; count<25; count++){
        ledOFF(GPIO1, count);
		delay(1000, TIMER2);// Pequeno delay entre desligar cada LED
	}*/
	ledOFF(GPIO2, 1);// Desliga o LED do pino 28

	// Envia mensagem via UART indicando que a entrada GPIO foi inicializada
	//uartGetString(UART0, "GPIO INPUT INICIALIZADO\n", 25);

// Loop principal que controla os LEDs baseado no estado atual e no botão pressionado
	while(true){
					switch (op) {
						case SEQ1:				// SEQ1: Pisca o LED no pino 21
							ledON(GPIO1, pino);
							delay(1000000, TIMER2);

							ledOFF(GPIO1, pino);
							delay(1000000, TIMER2);

							if(button_press(GPIO1, 16))
								op=SEQ2;
						break;
						case SEQ2:				// SEQ2: Pisca o LED no pino 22
							ledON(GPIO1, pino+1);
							delay(1000000, TIMER2);

							ledOFF(GPIO1, pino+1);
							delay(1000000, TIMER2);	

							// Se o botão no pino 12 for pressionado, avança para SEQ3
							if(button_press(GPIO1, 16))
								op=SEQ3;
						break;
						case SEQ3:				// SEQ3: Pisca o LED no pino 23
							ledON(GPIO1, pino+2);
							delay(1000000, TIMER2);

							ledOFF(GPIO1, pino+2);
							delay(1000000, TIMER2);	

							if(button_press(GPIO1, 16))
								op=SEQ4;
						break;
						case SEQ4:				// SEQ4: Pisca o LED no pino 24
							ledON(GPIO1, pino+3);
							delay(1000000, TIMER2);

							ledOFF(GPIO1, pino+3);
							delay(1000000, TIMER2);

							if(button_press(GPIO1, 16))
								op=SEQ5;//
						break;
						case SEQ5:				// SEQ5: Pisca o LED no pino 28
							ledON(GPIO1, 21);
							delay(1000000, TIMER2);

							ledOFF(GPIO1, 21);
							delay(1000000, TIMER2);

							if(button_press(GPIO1, 16))
								op=SEQ6;//
						break;
						case SEQ6: 				// SEQ6: Pisca todos os LEDs juntos (pinos 21 a 24 e 28)
							for(count=pino; count<25; count++)
								ledON(GPIO1, count);
							ledON(GPIO2, 1);
							delay(1000000, TIMER2);

							// Desliga todos os LEDs juntos
							for(count=pino; count<25; count++)
								ledOFF(GPIO1, count);
							ledOFF(GPIO2, 1);
							delay(1000000, TIMER2);

							if(button_press(GPIO1, 16))
								op=SEQ1;
						break;
						default:	
						break;
					}				/* -----  end switch  ----- */
	}
	
} // end of function main

void ledON(gpioMod mod, ucPinNumber pino){
	GpioSetPinValue(mod, pino, HIGH); // em nivel alto, chama a função que acende o led
	// se o parametro level for LOW entao apaga o led
}

void ledOFF(gpioMod mod, ucPinNumber pino){ // apaga o led
	GpioSetPinValue(mod, pino, LOW);
}

int button_press(gpioMod mod, ucPinNumber pino){
	return GpioGetPinValue(mod, pino); // retorna HIGH ou LOW
	// para verificar se o botão foi pressionado, se seu bit estiver setado(1)
	// entao ele retorna seu estado
}
