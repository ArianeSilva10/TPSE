		/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  14/08/2024
 *       Revision:  none
 *       Compiler:  arm-none-eabi-gcc
 *
 *         Author:  ariane
 * 		  Teacher:	Francisco Helder (FHC), helderhdw@gmail.com
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

//setenv app "setenv serverip 10.10.0.1; setenv ipaddr  10.10.0.2; tftp 0x80000000 appGpio.bin; go 0x80000000"
//run app

#include	"hw_types.h"
#include	"soc_AM335x.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
#define TIME2													1000000
#define TIME 													100000
#define TOGGLE          										(0x01u)

/* Watch Dog Timer */
#define  WDT_BASE 												0x44E35000
#define  WDT_WSPR 												0x44E35048
#define  WDT_WWPS 												0x44E35034

#define CM_PER_GPIO1											0xAC
#define CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE   				(0x2u)
#define CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK   			(0x00040000u)

#define CM_PER_GPIO2											0xB0
#define CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE   				(0x2u)
#define CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK   			(0x00040000u)


#define CM_PER_GPIO3											0xB4
#define CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE   				(0x2u)
#define CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK   			(0x00040000u)

#define GPIO_DATAIN												0x138

#define CM_conf_mcasp0_fsr										0x09A4

#define CM_conf_lcd_data2										0x8A8
#define CM_conf_lcd_data3										0x8AC


#define CM_conf_gpmc_ben1      	 								0x0878
#define CM_conf_gpmc_a5         								0x0854
#define CM_conf_gpmc_a6         								0x0858
#define CM_conf_gpmc_a7         								0x085C
#define CM_conf_gpmc_a8         								0x0860

#define CM_conf_gpmc_ad12         								0x0830 //botao2 gpio1_12

#define CM_conf_gpmc_ad13                                       0x0834 // pino 13
#define CM_conf_gpmc_ad15                                       0x083C //botao




#define GPIO_OE                 								0x134
#define GPIO_CLEARDATAOUT       								0x190
#define GPIO_SETDATAOUT         								0x194


unsigned int flagBlink;
unsigned int flagBlink2;
unsigned int flagBlink3;
unsigned int flagBlink4;
unsigned int flagBlink5;
unsigned int flagBlink6;


/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/
static void delay();
static void delay_low();
static void ledInit();
static void ledInit2();
static void ledToggle_ordem();
static void contrario();
static void ledToggle();
static void ledToggle2();
static void ledToggle3();
static void ledToggle4();
static void ledToggle5();
static bool buttonMode1(); // declarando estaticamente a função buttonMode 
static void buttonInit(); // declarando estaticamente a função buttonInit
void ledApaga();
static bool buttonMode2();


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */

void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}
int _main(void){

	//set watchdog
	/*-----------------------------------------------------------------------------
	 *  Desable WatchDog
	 *-----------------------------------------------------------------------------*/
	disableWdt();
	/*-----------------------------------------------------------------------------
	 *  initialize GPIO modules
	 *-----------------------------------------------------------------------------*/

	

	flagBlink=0;
	flagBlink2=0;	
	flagBlink3=0;
	flagBlink4=0;
	flagBlink5=0;
	flagBlink6=0;
	
  	
  	ledInit();
	ledInit2();

	buttonInit();
	
	
  
  	while (1){
		if (buttonMode1()){ // acionar essa sequencia caso o botão seja pressionado.
			ledToggle_ordem();
			ledApaga();
			delay_low();
		}else if (buttonMode2())
		{
			contrario();
			ledToggle5();
			delay_low();
			ledApaga();
			delay_low();
		}
		
		else{  // acionar essa sequencia caso o botão não seja pressionado.
			//ledApaga();
			ledToggle();
			ledToggle2();
			ledToggle3();
			ledToggle4();
			ledToggle5();
			delay_low();
		}	
		
		
	}

	return(0);
} /* ----------  end of function main  ---------- */


/*FUNCTION*-------------------------------------------------------
*
* Function Name : Delay
* Comments      :
*END*-----------------------------------------------------------*/
static void delay(){
	volatile unsigned int ra;
	for(ra = 0; ra < TIME; ra ++);
}

static void delay_low(){ // função de atraso do estado 
	volatile unsigned int ra;
	for(ra = 0; ra < TIME2; ra ++); // TIME é Tempo para o atraso.
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledInit
 *  Description:  
 * =====================================================================================
 */
void ledInit( ){
	
	unsigned int val_temp; 
	
	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO in clock module
	 *-----------------------------------------------------------------------------*/
	HWREG(SOC_CM_PER_REGS+CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;
	
	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module
	 *-----------------------------------------------------------------------------*/
 	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a5) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a6) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a7) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_a8) |= 7;
 
	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp &= ~(1<<21);
	val_temp &= ~(1<<22);
	val_temp &= ~(1<<23);
	val_temp &= ~(1<<24);

	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
	
}/* -----  end of function ledInit  ----- */

void ledInit2(){
	unsigned int val_temp;

	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO in clock module
	 *-----------------------------------------------------------------------------*/

	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module
	 *-----------------------------------------------------------------------------*/

	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_ad13) |= 7;

	/*-----------------------------------------------------------------------------
	 *  set pin direction 
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp &= ~(1<<29);
	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;
	
} /* -----  end of function ledInit2  ----- */

void buttonInit() {
	unsigned int val_temp;

	/*-----------------------------------------------------------------------------
	 *  configure clock GPIO 1 in clock module
	 *-----------------------------------------------------------------------------*/

	HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1) |= CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK | CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

	/*-----------------------------------------------------------------------------
	 * configure mux pin in control module
	 *-----------------------------------------------------------------------------*/

	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_ad15) |= 7;
	HWREG(SOC_CONTROL_REGS+CM_conf_gpmc_ad15) |= (1<<5);

	HWREG(SOC_CONTROL_REGS + CM_conf_gpmc_ad12) |= 7;
	HWREG(SOC_CONTROL_REGS + CM_conf_gpmc_ad12) |= (1<<5);

	/*-----------------------------------------------------------------------------
	 *  set pin direction as input
	 *-----------------------------------------------------------------------------*/
	val_temp = HWREG(SOC_GPIO_1_REGS+GPIO_OE);
	val_temp |= (1<<15);
	val_temp |= (1<<12);
	HWREG(SOC_GPIO_1_REGS+GPIO_OE) = val_temp;

}


// init function buttonMode

bool buttonMode1() {

	
	if (HWREG(SOC_GPIO_1_REGS+GPIO_DATAIN) & (1<<15)){
		return true;
	}
	return false;
	
	
}

bool buttonMode2(){
	if (HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN) & (1<<12)){
		return true;
	}return false;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledToggle
 *  Description:  
 * =====================================================================================
 */
void ledToggle_ordem(){
		
		for (int i = 21; i < 25; i++)
		{
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<i);
			delay_low();
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<i);
			delay_low();
		}
		HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1 << 13;
		delay();
		HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<13);
		delay();

}/* -----  end of function ledToggle  ----- */

void ledToggle(){
		
		flagBlink ^= TOGGLE;

		if(flagBlink){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1<<21;
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = 1<<21;
		}
}/* -----  end of function ledToggle  ----- */

void ledToggle2(){

		flagBlink2^= TOGGLE;

		if(flagBlink2){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1<<22;
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = 1<<22;
		}
}/* -----  end of function ledToggle  ----- */
void ledToggle3(){

		flagBlink3^= TOGGLE;

		if(flagBlink3){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1<<23;
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = 1<<23;
		}
}/* -----  end of function ledToggle  ----- */
void ledToggle4(){

		flagBlink4^= TOGGLE;

		if(flagBlink4){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1<<24;
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = 1<<24;
		}
}/* -----  end of function ledToggle  ----- */
void ledToggle5(){

		flagBlink5^= TOGGLE;

		if(flagBlink5){
			HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1<<13;
		}else{
			HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = 1<<13;
		}

}
void ledApaga(){
	HWREG(SOC_GPIO_1_REGS + GPIO_CLEARDATAOUT) = 1<<13;
}
/* -----  end of function ledToggle  -----  */
void contrario(){
	HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = 1 << 13;
		delay_low();
	HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<13);
		delay_low();

	for (int i = 24; i >= 21; i--)
	{
		HWREG(SOC_GPIO_1_REGS+GPIO_SETDATAOUT) = (1<<i);
		delay();
		HWREG(SOC_GPIO_1_REGS+GPIO_CLEARDATAOUT) = (1<<i);
		delay();
	}
}

