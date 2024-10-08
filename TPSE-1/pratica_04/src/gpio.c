#include "gpio.h"
#define CM_PER_GPIO_CLKCTRL_MODULEMODE_ENABLE   				(0x2u) // Valor pra habilitar o modulo do clock do GPIO1
#define CM_PER_GPIO_CLKCTRL_OPTFCLKEN_GPIO_GDBCLK   			(0x00040000u) // Valor para habilitar o clock



/*-----------------------------------------------------------------------------
 * insert [pin, mod], get CM_conf adress
 * Datasheet 4.3.2
 *-----------------------------------------------------------------------------*/
static const CONTROL_MODULE GPIO_CTRL_MODULE_ARRAY[32][4] = {
    //p0                          //p1                          //p2                          //p3
   {CM_conf_mdio                 ,CM_conf_gpmc_ad0             ,CM_conf_gpmc_csn3            ,CM_conf_mii1_col       },//.0
   {CM_conf_mdc                  ,CM_conf_gpmc_ad1             ,CM_conf_gpmc_clk             ,CM_conf_mii1_crs       },//.1
   {CM_conf_spi0_sclk            ,CM_conf_gpmc_ad2             ,CM_conf_gpmc_advn_ale        ,CM_conf_mii1_rx_er     },//.2
   {CM_conf_spi0_d0              ,CM_conf_gpmc_ad3             ,CM_conf_gpmc_oen_ren         ,CM_conf_mii1_tx_en     },//.3
   {CM_conf_spi0_d1              ,CM_conf_gpmc_ad4             ,CM_conf_gpmc_wen             ,CM_conf_mii1_rx_dv     },//.4
   {CM_conf_spi0_cs0             ,CM_conf_gpmc_ad5             ,CM_conf_gpmc_ben0_cle        ,CM_conf_i2c0_sda       },//.5
   {CM_conf_spi0_cs1             ,CM_conf_gpmc_ad6             ,CM_conf_lcd_data0            ,CM_conf_i2c0_scl       },//.6
   {CM_conf_ecap0_in_pwm0_out    ,CM_conf_gpmc_ad7             ,CM_conf_lcd_data1            ,CM_conf_emu0           },//.7
   {CM_conf_lcd_data12           ,CM_conf_uart0_ctsn           ,CM_conf_lcd_data2            ,CM_conf_emu1           },//.8
   {CM_conf_lcd_data13           ,CM_conf_uart0_rtsn           ,CM_conf_lcd_data3            ,CM_conf_mii1_tx_clk    },//.9
   {CM_conf_lcd_data14           ,CM_conf_uart0_rxd            ,CM_conf_lcd_data4            ,CM_conf_mii1_rx_clk    },//.10
   {CM_conf_lcd_data15           ,CM_conf_uart0_txd            ,CM_conf_lcd_data5            ,-1                     },//.11
   {CM_conf_uart1_ctsn           ,CM_conf_gpmc_ad12            ,CM_conf_lcd_data6            ,-1                     },//.12
   {CM_conf_uart1_rtsn           ,CM_conf_gpmc_ad13            ,CM_conf_lcd_data7            ,CM_conf_usb1_drvvbus   },//.13
   {CM_conf_uart1_rxd            ,CM_conf_gpmc_ad14            ,CM_conf_lcd_data8            ,CM_conf_mcasp0_aclkx   },//.14
   {CM_conf_uart1_txd            ,CM_conf_gpmc_ad15            ,CM_conf_lcd_data9            ,CM_conf_mcasp0_fsx     },//.15
   {CM_conf_mii1_txd3            ,CM_conf_gpmc_a0              ,CM_conf_lcd_data10           ,CM_conf_mcasp0_axr0    },//.16
   {CM_conf_mii1_txd2            ,CM_conf_gpmc_a1              ,CM_conf_lcd_data11           ,CM_conf_mcasp0_ahclkr  },//.17
   {CM_conf_usb0_drvvbus         ,CM_conf_gpmc_a2              ,CM_conf_mii1_rxd3            ,CM_conf_mcasp0_aclkr   },//.18
   {CM_conf_xdma_event_intr0     ,CM_conf_gpmc_a3              ,CM_conf_mii1_rxd2            ,CM_conf_mcasp0_fsr     },//.19
   {CM_conf_xdma_event_intr1     ,CM_conf_gpmc_a4              ,CM_conf_mii1_rxd1            ,CM_conf_mcasp0_axr1    },//.20
   {CM_conf_mii1_txd1            ,CM_conf_gpmc_a5              ,CM_conf_mii1_rxd0            ,CM_conf_mcasp0_ahclkx  },//.21
   {CM_conf_gpmc_ad8             ,CM_conf_gpmc_a6              ,CM_conf_lcd_vsync            ,-1                     },//.22
   {CM_conf_gpmc_ad9             ,CM_conf_gpmc_a7              ,CM_conf_lcd_hsync            ,-1                     },//.23
   {-1                           ,CM_conf_gpmc_a8              ,CM_conf_lcd_pclk             ,-1                     },//.24
   {-1                           ,CM_conf_gpmc_a9              ,CM_conf_lcd_ac_bias_en       ,-1                     },//.25
   {CM_conf_gpmc_ad10            ,CM_conf_gpmc_a10             ,CM_conf_mmc0_dat3            ,-1                     },//.26
   {CM_conf_gpmc_ad11            ,CM_conf_gpmc_a11             ,CM_conf_mmc0_dat2            ,-1                     },//.27
   {CM_conf_mii1_txd0            ,CM_conf_gpmc_ben1            ,CM_conf_mmc0_dat1            ,-1                     },//.28
   {CM_conf_rmii1_ref_clk        ,CM_conf_gpmc_csn0            ,CM_conf_mmc0_dat0            ,-1                     },//.29
   {CM_conf_gpmc_wait0           ,CM_conf_gpmc_csn1            ,CM_conf_mmc0_clk             ,-1                     },//.30
   {CM_conf_gpmc_wpn             ,CM_conf_gpmc_csn2            ,CM_conf_mmc0_cmd             ,-1                     },//.31
};



/*-----------------------------------------------------------------------------
 *  Function prototype
 *-----------------------------------------------------------------------------*/

void Init_module_gpio(gpioMod mod){
// Usado para escrever valores em registradores específicos para habilitar o clock para o módulo GPIO correspondente.
	switch (mod)
	{
	case GPIO0:
		return;
	break;
	case GPIO1:
		HWREG(SOC_CM_PER_REGS + CKM_PER_GPIO1_CLKCTRL) |= CM_PER_GPIO_CLKCTRL_OPTFCLKEN_GPIO_GDBCLK | CM_PER_GPIO_CLKCTRL_MODULEMODE_ENABLE;
	break; // inicalizo o modulo de gpio para multiplexacao e habilitar o clock
	case GPIO2:
		HWREG(SOC_CM_PER_REGS + CKM_PER_GPIO2_CLKCTRL) |= CM_PER_GPIO_CLKCTRL_OPTFCLKEN_GPIO_GDBCLK | CM_PER_GPIO_CLKCTRL_MODULEMODE_ENABLE;
	break;
	case GPIO3:
		HWREG(SOC_CM_PER_REGS + CKM_PER_GPIO3_CLKCTRL) |= CM_PER_GPIO_CLKCTRL_OPTFCLKEN_GPIO_GDBCLK | CM_PER_GPIO_CLKCTRL_MODULEMODE_ENABLE;
	break;
	default:
	return;
		break;
	}
}

void Init_pin_gpio(gpioMod modulo, ucPinNumber pino, Direction direcao){
	CONTROL_MODULE nome_pino_especifico = GPIO_CTRL_MODULE_ARRAY[pino][modulo]; // Converter o pino em um módulo de controle apropriado

// configuracao de modo para o pino
	HWREG(SOC_CONTROL_REGS + nome_pino_especifico) |= MODE_7;

	if (direcao == INPUT) // se for de entrada entao inicializo o pino com input
	{
		HWREG(SOC_CONTROL_REGS + nome_pino_especifico) |= (1<<5);
//Manipula o registrador de controle para o pino específico, configurando-o no modo correto.
/*Input enable value for the PAD: Este é um valor de configuração que define se o pino está habilitado 
para funcionar como uma entrada (input) de dados. O termo "PAD" aqui refere-se ao hardware físico que se 
conecta ao pino, que pode ser configurado para receber sinais externos. Página 1515, manual.*/
	}else if (direcao == OUTPUT)
	{
		HWREG(SOC_CONTROL_REGS + nome_pino_especifico) &= ~(1 <<5);
	}
}

void Set_direction_pin_gpio(gpioMod mod, Direction direcao, ucPinNumber pino){
//Configura a direção de um pino GPIO específico como INPUT ou OUTPUT, ajustando o registrador GPIO_OE.
	unsigned int val_temp, reg_base;
	
	switch (mod)
	{
	case GPIO0:
		reg_base = SOC_GPIO_0_REGS;
	break;
	case GPIO1:
		// direciono o pino como input ou output
			reg_base = SOC_GPIO_1_REGS;
	break;
	case GPIO2:
			reg_base = SOC_GPIO_2_REGS;
	break;
	case GPIO3:
			reg_base = SOC_GPIO_3_REGS;
	break;
	default:
		return;
		break;
	}
	val_temp = HWREG(reg_base + GPIO_OE);

	if (direcao == INPUT)
	{
		val_temp |= (1<<pino);
	}else
	{
		val_temp &= ~(1<<pino);
	}
	
	HWREG(reg_base + GPIO_OE) = val_temp;
}

unsigned int Get_direction_pin_gpio(gpioMod mod, unsigned char pino){
// Obtém a direção atual de um pino GPIO, verificando se ele é uma entrada ou saída.
	unsigned int val_temp;

	switch (mod)
	{
	case GPIO0:
		val_temp = HWREG(SOC_GPIO_0_REGS + GPIO_OE);
		break;
	case GPIO1:
		val_temp = HWREG(SOC_GPIO_1_REGS + GPIO_OE);
	break;
	case GPIO2:
		val_temp = HWREG(SOC_GPIO_2_REGS + GPIO_OE);
	break;
	case GPIO3:
		val_temp = HWREG(SOC_GPIO_3_REGS + GPIO_OE);
	break;
	default:
		return -1;
	break;
	}
	if (val_temp & (1<< pino)) // retorno se é pra entrada ou saida
	{
		return(INPUT);
	}else{ return(OUTPUT);
	}
}

void GpioSetPinValue(gpioMod mod , ucPinNumber pino, Level valor){
//  Define o valor de um pino GPIO como HIGH ou LOW, usando os registradores GPIO_SETDATAOUT e GPIO_CLEARDATAOUT.	
	if (valor == HIGH) // caso seja saida acendo/apago led
	{
		switch (mod)
		{
		case GPIO0:
			HWREG(SOC_GPIO_0_REGS + GPIO_SETDATAOUT) = (1 << pino);
			break;
		case GPIO1:
			HWREG(SOC_GPIO_1_REGS + GPIO_SETDATAOUT) = (1 << pino);
			break;
		case GPIO2:
			HWREG(SOC_GPIO_2_REGS + GPIO_SETDATAOUT) = (1<<pino);
			break;
		case GPIO3:
			HWREG(SOC_GPIO_3_REGS + GPIO_SETDATAOUT) = (1<<pino);
			break;
		default:
		return;
			break;
		}
	} else
	{
		switch (mod)
		{
		case GPIO0:
			HWREG(SOC_GPIO_0_REGS + GPIO_CLEARDATAOUT) = (1 << pino);
		break;
		case GPIO1:
			HWREG(SOC_GPIO_1_REGS + GPIO_CLEARDATAOUT) = (1 << pino);
		break;
		case GPIO2:
			HWREG(SOC_GPIO_2_REGS + GPIO_CLEARDATAOUT) = (1<<pino);
		break;
		case GPIO3:
			HWREG(SOC_GPIO_3_REGS + GPIO_CLEARDATAOUT) = (1<<pino);
		break;
		default:
		return;
			break;
		}
	}
}

unsigned int GpioGetPinValue(gpioMod mod, ucPinNumber pino){
// Retorna o valor atual de um pino GPIO (HIGH ou LOW), verificando o registrador GPIO_DATAIN.
	unsigned int val_temp;
	// função para o botão, seu retorno define se o botão foi pressionado ou não
	switch (mod)
	{
	case GPIO0:
		val_temp = HWREG(SOC_GPIO_0_REGS + GPIO_DATAIN);
		break;
	case GPIO1:
		val_temp = HWREG(SOC_GPIO_1_REGS + GPIO_DATAIN);
		break;
	case GPIO2:
		val_temp = HWREG(SOC_GPIO_2_REGS + GPIO_DATAIN);
		break;
	case GPIO3:
		val_temp = HWREG(SOC_GPIO_3_REGS + GPIO_DATAIN);
		break;
	default:
	return -1;
		break;
	}

	if (val_temp & (1 << pino))
	{
		return(HIGH);
	}else
	{
		return(LOW);
	}
}