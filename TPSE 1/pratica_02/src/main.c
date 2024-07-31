		/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/02/2017 20:05:55
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

//setenv app "setenv serverip 10.10.0.1; setenv ipaddr  10.10.0.2; tftp 0x80000000 appGpio.bin; go 0x80000000"
//run app

#include	"hw_types.h" //Contém definições de tipos de hardware e macros.
#include	"soc_AM335x.h" //Contém definições específicas para o SoC (System on Chip) AM335x.

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIME													1000000 //Tempo para o atraso.
#define TOGGLE          										(0x01u) // Alterna o estado do  led

#define CM_PER_GPIO1											0xAC // Endereço do registrador de controle do modulo de clock para GPIO1
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE   				(0x2u) // Valor pra habilitar o modulo do clock do GPIO1
#define CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK   			(0x00040000u) // Valor para habilitar o clock

#define CM_conf_gpmc_ben1      	 								0x0878 // Endereço dos registradores de configuração de multiplexação dos pinos
#define CM_conf_gpmc_a5         								0x0854 // Endereço dos registrad1ores de configuração de multiplexação dos pinos
#define CM_conf_gpmc_a6											0x0858
#define CM_conf_gpmc_a7											0x085C
#define CM_conf_gpmc_a8											0x0860
#define CM_conf_gpmc_be1n  										0x0878

#define GPIO_OE                 								0x134 //Registro de configuração do pino como entrada/saida
#define GPIO_CLEARDATAOUT       								0x190 //Registro para limpar os dados de saida da GPIO
#define GPIO_SETDATAOUT         								0x194 // Registro para definir dados(setar) de saida da GPIO


unsigned int flagBlink; //variavel usada para manter o estado do led (aceso/apagado)

/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void delay(); //Função para criar um atraso
static void ledInit(); //Função que Inicializa o estado do led
static void ledToggle(); //Função que Alterna o estado do led

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int _main(void){

	flagBlink=0;	//init flag ----- variavel usada para manter o estado do led (aceso/apagado)
  	
	/* Configure the green LED control pin. */
  	ledInit(); // Chama a função que configura o led, assim o inicializando
  
  	while (1){ // looping para ir alternando o estado do led entre aceso e apagado
    	/* Change the state of the green LED. */
    	ledToggle(); // Alterna o estado entre aceso e apagado
		delay(); //cria o atraso desse estado
		ledToggle(); // Alterna o estado entre aceso e apagado
		delay(); //cria o atraso desse estado
	}

	return(0);
} /* ----------  end of function main  ---------- */


/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      : O `volatile` indica que a variável `ra` pode ser modificada em outro contexto 
(por exemplo, por hardware), evitando que o compilador otimize o loop.

*END*-----------------------------------------------------------*/
static void delay(){ // função de atraso do estado 
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++); // TIME é Tempo para o atraso.
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledInit
 *  Description:  
 * =====================================================================================
 */
// Função inicializadora do led
void ledInit( ){
	
	unsigned int val_temp; 	
	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO in clock module -------Configura o clock para o modulo GPIO1
	 *-----------------------------------------------------------------------------*/
	HWREG(SOC_CM_PER_REGS+CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
	
	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module ---------Configura o multiplexador para usar o pino específico para o led
	 *-----------------------------------------------------------------------------*/
   	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a5) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a6) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a7) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a8) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_be1n)  |= 7;
	/*-----------------------------------------------------------------------------
	 *  set pin direction ------Configura a direção o pino GPIO1_21 como saida
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp &= ~(1<<21);
	val_temp &= ~(1<<22);
	val_temp &= ~(1<<23);
	val_temp &= ~(1<<24);
	val_temp &= ~(1<<28);
	
	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
		
}/* -----  end of function ledInit  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledToggle
 *  Description:  
 * =====================================================================================
 */
// Função de alternancia do led
void ledToggle(){
		
		flagBlink ^= TOGGLE;

		if(flagBlink){
			/*unsigned  reg = HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT);
			reg |= 1 << 21;
			reg |= 1 << 22;
			reg |= 1 << 23;
			reg |= 1 << 24;
			reg |= 1 << 28;*/
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (0x1<<21); //Se  flagBlink é 1, define o pino 21 como alto (acende o LED).
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<22);
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<23);
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<24);
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1 << 28;
		}
		else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (0x1<<21); // Se flagBlink é 0, define o pino 21 como baixo (apaga o LED).
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<22); 
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<23); 
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<24); 
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<28);
		}

}/* -----  end of function ledToggle  ----- */










