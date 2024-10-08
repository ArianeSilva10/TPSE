/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Agosto, 2024
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Francisca Ariane dos Santos da Silva, francisca.silva@alu.ufc.br
 *   Organization:  UFC-Quixadá
 *
 * =====================================================================================
 */

#include "bbb_regs.h"
#include "hw_types.h"

#define DELAY_USE_INTERRUPT			1  // define se o delay usará interrupções (1 = sim, 0 = não).

/**
 * \brief   This macro will check for write POSTED status
 *
 * \param   reg          Register whose status has to be checked
 *
 *    'reg' can take the following values \n
 *    DMTIMER_WRITE_POST_TCLR - Timer Control register \n
 *    DMTIMER_WRITE_POST_TCRR - Timer Counter register \n
 *    DMTIMER_WRITE_POST_TLDR - Timer Load register \n
 *    DMTIMER_WRITE_POST_TTGR - Timer Trigger register \n
 *    DMTIMER_WRITE_POST_TMAR - Timer Match register \n
 *
 **/
#define DMTimerWaitForWrite(reg)   \
            if(HWREG(DMTIMER_TSICR) & 0x4)\
            while((reg & HWREG(DMTIMER_TWPS))); //Essa macro verifica se uma operação de escrita 
			//para um registro de timer específico foi postada e aguarda até que a operação seja
			// concluída antes de prosseguir.


int flag_timer = false; // Indica o estado do timer (1 = ativo, 0 = inativo).
int flag_gpio12 = 0; // Usada para rastrear o estado de um GPIO específico.
unsigned int frequence = 20; // Define a frequência (ou delay) entre operações de LED.


typedef enum _pinNum{
//  define nomes simbólicos para os pinos do GPIO, facilitando a leitura do código.
	PIN1=1,
	PIN2=3,
	PIN3=4, 
}pinNum;

void timerIrqHandler(void);
void gpioIsrHandlerA(void);
void gpioIsrHandlerB(void);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  disableWdt
 *  Description:  Esta função desativa o Watchdog Timer (WDT), que é um sistema de 
 * segurança que reinicia o sistema caso ele trave.
 * =====================================================================================
 */
void disableWdt(void){
	HWREG(WDT_WSPR) = 0xAAAA;
	while((HWREG(WDT_WWPS) & (1<<4)));
	
	HWREG(WDT_WSPR) = 0x5555;
	while((HWREG(WDT_WWPS) & (1<<4)));
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  putCh
 *  Description:  Envia um caractere através da UART.
 * =====================================================================================
 */
void putCh(char c){
	while(!(HWREG(UART0_LSR) & (1<<5)));/*A função começa verificando se o registrador 
	THR está disponível (ou seja, vazio e pronto para receber um novo caractere). Ela faz
	isso monitorando o bit THR Empty no registro LSR. Enquanto o bit não estiver definido
	(0), o loop while impede que o código avance. O while termina, pois a condição se tornou
	0 (ou seja, falsa), e o código avança para a próxima linha.
	
	Esta operação realiza uma máscara bit a bit, 
	verificando se o bit 5 (THR Empty) do registro LSR está definido como 1. O bit 5 
	indica se o registrador THR (Transmitter Holding Register) está vazio e pronto para 
	receber um novo caractere para transmissão.
	O loop while continua verificando o estado do bit 5 até que ele se torne 1, indicando
	que o THR está vazio. Isso significa que a UART está pronta para enviar um novo 
	caractere.
	*/

	HWREG(UART0_THR) = c; /*Quando o THR estiver vazio (bit 5 = 1), o loop while termina, 
	e o caractere c é então escrito no registrador THR. Isso inicia o processo de 
	transmissão do caractere através da interface UART.
	
	Este comando envia o caractere c através da UART, transmitindo-o 
	para o dispositivo conectado.*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getCh
 *  Description:  Recebe um caractere da UART.
 * =====================================================================================
 */
char getCh(){
	while(!(HWREG(UART0_LSR) & (1<<0)));/*A condição do while será verdadeira enquanto não
	houver dados disponíveis (ou seja, enquanto o bit 0 de UART0_LSR for 0). Assim, o loop
	continua até que o bit 0 se torne 1, indicando que um dado foi recebido e está pronto
	para ser lido. Quando se tornar 1, o while se torna falso e pula para proxima linha*/

	return(HWREG(UART0_RHR));/*Depois que o loop termina, significa que um dado foi recebido.
	O UART0_RHR (Receiver Holding Register) contém o caractere recebido.
	A função então retorna o valor de HWREG(UART0_RHR), que é o caractere lido da UART.*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  putString
 *  Description: Envia uma string de caracteres. 
 * =====================================================================================
 */
int putString(char *str, unsigned int length){
	for(int i = 0; i < length; i++){
    	putCh(str[i]);/*
    A função putCh() é chamada com o caractere atual da string str[i]. A função putCh() 
	é responsável por enviar um único caractere pela UART. Assim, putString() envia a 
	string caractere por caractere.*/
	}
	return(length);/*Retorna o comprimento da string que foi enviada.*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getString
 *  Description:   Recebe uma string de caracteres.
 * =====================================================================================
 */
int getString(char *buf, unsigned int length){
	for(int i = 0; i < length; i ++){
    	buf[i] = getCh(); /*A função getCh() é chamada para ler um caractere da UART. A
		função getCh() espera até que um caractere esteja disponível e então o retorna.
		O caractere lido é armazenado na posição atual do buffer buf[i].*/
   	}
	return(length);/*Após ler todos os caracteres e armazená-los no buffer, a função 
	retorna o comprimento da string que foi lida.*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerEnable
 *  Description:  ativa o timer
 * =====================================================================================
 */
void timerEnable(){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1); /*DMTimerWaitForWrite(0x1): Esta macro é usada 
	para aguardar até que a escrita no registrador especificado (0x1 neste caso) tenha sido 
	completada. O argumento 0x1 refere-se ao TCLR, o registrador de controle do temporizador. 
	A macro verifica se a operação de escrita nesse registrador foi finalizada antes de 
	prosseguir.*/

    /* Start the timer */
    HWREG(DMTIMER_TCLR) |= 0x1; /*O operador |= 0x1 configura o bit 0 do registrador TCLR 
	para 1, que é o bit responsável por iniciar o temporizador.
	Definindo esse bit para 1, o temporizador é ativado e começa a contar.*/

}/* -----  end of function timerEnable  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerDisable
 *  Description:  desativa o timer
 * =====================================================================================
 */
void timerDisable(){
    /* Wait for previous write to complete in TCLR */
	DMTimerWaitForWrite(0x1);/*verifica se a operação de escrita nesse registrador
	foi finalizada antes de continuar*/

    /* Start the timer */
    HWREG(DMTIMER_TCLR) &= ~(0x1); /*limpa o bit 0 do registrador TCLR, fazendo-o 
	parar de contar e desativar o temporizador.*/
}/* -----  end of function timerEnable  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  delay
 *  Description:  implementa um delay em milissegundos. Se "DELAY_USE_INTERRUPT" for 
 * definido, o delay usa interrupções; caso contrário, usa uma contagem manual
 * =====================================================================================
 */
void delay(unsigned int mSec){
#ifdef DELAY_USE_INTERRUPT
    unsigned int countVal = TIMER_OVERFLOW - (mSec * TIMER_1MS_COUNT);/*Calcula o valor 
	que deve ser carregado no registrador do temporizador para gerar o atraso desejado. 
	TIMER_OVERFLOW é o valor máximo que o temporizador pode contar antes de overflow, e 
	TIMER_1MS_COUNT é o número de contagens por milissegundo.*/

   	/* Wait for previous write to complete */
	DMTimerWaitForWrite(0x2); /*Garante que qualquer escrita anterior no registrador do 
	temporizador (0x2) tenha sido concluída. 0x2 - tcrr (0verflow)*/

    /* Load the register with the re-load value */
	HWREG(DMTIMER_TCRR) = countVal; /*Carrega o valor calculado no registrador de contagem 
	do temporizador.*/
	
	flag_timer = false;/*Inicializa a flag que será usada para indicar que o atraso foi 
	completado.*/

    /* Enable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_SET) = 0x2; /*habilita a interrupção do temporizador. Página 1851
	, manual*/

    /* Start the DMTimer */
	timerEnable(); /*ativa o timer*/

	while(flag_timer == false); /** se flag_timer for true, pula pra proxima linha ou seja, 
	se o atraso já está completo */

    /* Disable the DMTimer interrupts */
	HWREG(DMTIMER_IRQENABLE_CLR) = 0x2; /*se já estiver completo, desebilita a interrupção
	do temporizador. Página 1853, manual*/

#else /*Quando DELAY_USE_INTERRUPT não está definido, 
		a função utiliza polling para gerar o atraso. Isso significa que o processador fica 
		ocupado esperando até que o temporizador atinja um valor específico.*/
    while(mSec != 0){ /*Loop que continua até que o número de milissegundos (mSec) tenha sido 
		contado.*/
        /* Wait for previous write to complete */
        DMTimerWaitForWrite(0x2);/*Garante que qualquer escrita anterior no registrador do 
		temporizador (0x2) tenha sido concluída. 0x2 - tcrr (0verflow)*/

        /* Set the counter value. */
        HWREG(DMTIMER_TCRR) = 0x0; /*Configura o valor inicial do registrador do temporizador 
		para 0. Página 4462, manual*/

        timerEnable(); /*inicia o temporizador*/

        while(HWREG(DMTIMER_TCRR) < TIMER_1MS_COUNT);/*Espera até que o contador do 
		temporizador atinja o valor correspondente a 1 milissegundo.*/

        /* Stop the timer */
        HWREG(DMTIMER_TCLR) &= ~(0x00000001u); /*Para o temporizador(congela), página 4461, 
		manual*/

        mSec--; /*Reduz o número de milissegundos restantes.*/
    }
#endif
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerSetup
 *  Description:  configura o timer, habilitando o clock e configurando interrupções
 * =====================================================================================
 */
void timerSetup(void){
     /*  Clock enable for DMTIMER7 pagina 1280 */
    HWREG(CM_PER_TIMER7_CLKCTRL) |= 0x2; /*ativa o clock do registrador timer 7*/

	/*  Check clock enable for DMTIMER7 TRM 8.1.12.1.25 */    
    while((HWREG(CM_PER_TIMER7_CLKCTRL) & 0x3) != 0x2); /*Lê o valor do registrador 
	CM_PER_TIMER7_CLKCTRL e faz uma operação AND bit a bit com 0x3. Esta operação isola 
	os dois bits menos significativos do registrador, que indicam o status do clock.
	!= 0x2: Verifica se o valor dos dois bits isolados é diferente de 0x2. O valor 0x2 
	indica que o clock foi ativado corretamente. Enquanto o valor dos bits não for 0x2, 
	o loop while continua executando, aguardando a ativação completa do clock.*/

#ifdef DELAY_USE_INTERRUPT
    /* Interrupt mask */
    HWREG(INTC_MIR_CLEAR2) |= (1<<31);//(95 --> Bit 31 do 3º registrador (MIR CLEAR2)), 
	//página 545, manual
	/*Se a interrupção estiver em uso, a máscara de interrupção para o DMTIMER7 é 
	configurada. O bit 31 do registrador INTC_MIR_CLEAR2 é definido para permitir que 
	o temporizador DMTIMER7 gere interrupções. */
#endif
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gpioSetup
 *  Description: configura o GPIO, habilitando o clock e desativando máscaras de 
 * interrupção 
 * =====================================================================================
 */
void gpioSetup(){
	/* set clock for GPIO1, TRM 8.1.12.1.31 */
	HWREG(CM_PER_GPIO1_CLKCTRL) = 0x40002;
	HWREG(CM_PER_GPIO2_CLKCTRL) = 0x40002;  //ativa o clock do gpio1, pagina 1284

	// ==
	//HWREG(CM_PER_GPIO2_CLKCTRL) |= 0x40000; // bit 18 setado
	//HWREG(CM_PER_GPIO2_CLKCTRL) |= 0x2; // ativa(enable)

    /* Interrupt mask */


    HWREG(INTC_MIR_CLEAR3) |= (1<<2);//(98 --> Bit 2 do 4º registrador (MIR CLEAR3)) gpio1A
	HWREG(INTC_MIR_CLEAR3) |= (1<<3);//(99 --> Bit 3 do 4º registrador (MIR CLEAR3)) gpio1B
	/*Limpar a máscara de interrupção para que as interrupções dos números 98 e 99 possam 
	ser atendidas.*/
}
/* 98 >> 5 = 3
98/2
49/2
24/2
12/2
6/2
3 */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  butConfig
 *  Description:  configura botões de entrada, incluindo interrupções para detecção de 
 * borda de subida
 * =====================================================================================
 */
void butConfig ( ){
    /*  configure pin mux for input GPIO  botão 1: */
    HWREG(CM_PER_GPMCA0_REGS) |= 0x2F; // == (1<<5)|(1<<3)|(0x7); 10 1111, gpio1_16
	/*0010 0000 | (1<<5) modo entrada
	  0000 1000 | (1<<3) habilita o pull up
	  0000 0111 | 0x7 função GPIO
	  ==========
	  0010 1111
	    2   F
	  */
	/*Configura a multiplexação do pino 16 do GPIO1 para ser usado como entrada*/

	HWREG(CM_conf_gpmc_be1n) |= 0X2F; /*Configura a multiplexação do pino 28 do 
	GPIO1 para ser usado como entrada*/
    
    HWREG(GPIO1_OE) |= 1<<16; /*configura o pino 16 do gpio1 como entrada*/
	HWREG(GPIO1_OE) |= 1<<28; /*configura o pino 28 do gpio1 como entrada*/

    /* Setting interrupt GPIO pin. */
	HWREG(GPIO1_IRQSTATUS_SET_0) |= 1<<16; /*Habilita a interrupção para o pino 16*/
	HWREG(GPIO1_IRQSTATUS_SET_1) |= 1<<28; /*Habilita a interrupção para o pino 28*/
	// associar um pino a determinado grupo de interrupção

  	/* Enable interrupt generation on detection of a rising edge.*/
	HWREG(GPIO1_RISINGDETECT) |= 1<<16;	/*habilita a detecção de borda de subida
	no pino 16 do gpio1*/

	HWREG(GPIO1_RISINGDETECT) |= 1<<28;	/*habilita a detecção de borda de subida
	no pino 28 do gpio1, pull down*/
	HWREG(GPIO1_DEBOUNCENABLE) |= (1<<16); //pull up
}/* -----  end of function butConfig  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledConfig
 *  Description:  configura os LEDs como saída, configurando os pinos adequados
 * =====================================================================================
 */
void ledConfig ( ){ /*ATENÇÃO!!!!!! CONFIGURAR O GPIO_2*/
    /*  configure pin mux for output GPIO */
    HWREG(CM_PER_GPMC_CLK_REGS) |= 0x7;
	HWREG(CM_PER_GPMC_OEN_REN_REGS) |= 0x7;
    HWREG(CM_PER_GPMC_WEn_REGS) |= 0x7;



    /* clear pin 23 and 24 for output, leds USR3 and USR4, TRM 25.3.4.3 */

	HWREG(GPIO2_OE) &= ~(1<<1);
	HWREG(GPIO2_OE) &= ~(1<<3);
	HWREG(GPIO2_OE) &= ~(1<<4);

}/* -----  end of function ledConfig  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readBut
 *  Description:  lê o estado do botão
 * =====================================================================================
 */
unsigned int readBut ( ){
	unsigned int temp;
	temp = HWREG(GPIO1_DATAIN)&0x1000;
	
	return(temp);
}/* -----  end of function readBut  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledOff
 *  Description:  desliga o led
 * =====================================================================================
 */
void ledOff(pinNum pin){
	switch (pin) {
		case PIN1:
			HWREG(GPIO2_CLEARDATAOUT) = (1<<LED_EX_G2_1);
		break;
		case PIN2:	
			HWREG(GPIO2_CLEARDATAOUT) = (1<<LED_EX_G2_3);
		break;
		case PIN3:	
			HWREG(GPIO2_CLEARDATAOUT) = (1<<LED_EX_G2_4);
		break;
		default:
		break;
	}/* -----  end switch  ----- */
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ledOn
 *  Description:  liga o led
 * =====================================================================================
 */
void ledOn(pinNum pin){
	switch (pin) {
		case PIN1:
			HWREG(GPIO2_SETDATAOUT) = (1<<LED_EX_G2_1);
		break;
		case PIN2:	
			HWREG(GPIO2_SETDATAOUT) = (1<<LED_EX_G2_3);
		break;
		case PIN3:	
			HWREG(GPIO2_SETDATAOUT) = (1<<LED_EX_G2_4);
		break;
		default:
		break;
	}/* -----  end switch  ----- */
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  gpioIsrHandler
 *  Description:  Essa função é chamada quando ocorre uma interrupção no GPIO (General 
 * Purpose Input/Output). Dependendo do número da interrupção (irq_number), ela executa 
 * ações específicas:

    Caso irq_number 95: Chama a função timerIrqHandler, que é responsável por lidar com 
	interrupções do timer.

    Caso irq_number 98:
        Limpa o sinal de interrupção no pino 12 do GPIO (HWREG(GPIO1_IRQSTATUS_0) = 0x1000).
        Exibe a mensagem "button A press!\n\r" na saída serial.
        Incrementa flag_gpio12, que controla a lógica de ativação dos LEDs. Se flag_gpio12 
		já for 2, ele é resetado para 0; caso contrário, é incrementado.

    Caso irq_number 99:
        Limpa o sinal de interrupção no pino 14 do GPIO (HWREG(GPIO1_IRQSTATUS_1) = 0x4000).
        Exibe a mensagem "button B press!\n\r" na saída serial.
        Ajusta o valor da variável frequence, que controla o tempo de delay entre as mudanças 
		de estado dos LEDs. Se a frequência for maior que 1000, ela é resetada para 100; 
		caso contrário, é incrementada em 200.
 * =====================================================================================
 */

void gpioIsrHandlerA(void){
	HWREG(GPIO1_IRQSTATUS_0) = (1<<16); // == (1<<14) 100 0000 0000 0000
	if(frequence > 100){
		frequence = 20;
	}else
	{
	frequence += 20;	
	}
}
 void gpioIsrHandlerB(void){
			HWREG(GPIO1_IRQSTATUS_1) = (1<<28); // == (1<<12) 1 0000 0000 0000
			/*Limpa a flag de interrupção para o GPIO no registrador de status de interrupção 
			0 (GPIO1_IRQSTATUS_0). 0x1000 (ou (1 << 12)) indica que a interrupção do pino 12 
			está sendo limpa.*/

		putString("button B press! toggle led\n\r",28); /*Exibe uma mensagem indicando que o botão A 
			foi pressionado. A função putString é responsável por enviar a mensagem para um 
			console ou outra interface de saída.*/

			if(flag_gpio12 >= 2){ /*flag_gpio12 é maior ou igual a 2, ele é redefinido para 0. 
			Caso contrário, é incrementado. Isso pode ser utilizado para alternar entre 
			diferentes estados ou para contar o número de vezes que o botão A foi pressionado.*/
				flag_gpio12 = 0;
			}else{
				flag_gpio12++;
			}
			/*A variável flag_gpio12 é usada para alternar entre três estados possíveis (0, 1, e 2) 
			com base no número de pressões do botão. Esse tipo de lógica é útil para criar um 
			comportamento cíclico ou alternar entre modos diferentes em resposta a eventos de 
			interrupção.*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  timerIrqHandler
 *  Description:  Essa função lida com interrupções geradas pelo timer:

    Limpa a interrupção: Define o bit apropriado no registro DMTIMER_IRQSTATUS para 
	indicar que a interrupção foi tratada.
    Define flag_timer como verdadeiro: Isso sinaliza que o evento do timer foi processado.
    Desativa o timer: Para o timer, utilizando a função timerDisable.
 * =====================================================================================
 */
void timerIrqHandler(void){

    /* Clear the status of the interrupt flags */
	HWREG(DMTIMER_IRQSTATUS) = 0x2; /*O registrador DMTIMER_IRQSTATUS é usado para indicar o 
	status das interrupções do temporizador. A escrita de um valor específico (neste caso, 
	0x2) no registrador geralmente serve para limpar ou reconhecer a interrupção.*/

	flag_timer = true; /* A variável flag_timer é configurada como true para indicar que o 
	evento de interrupção do temporizador foi detectado. */

    /* Stop the DMTimer */
	timerDisable(); /*desativa o temporizador*/

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ISR_Handler
 *  Description:  Essa é a função principal de tratamento de interrupções. Quando uma 
 * interrupção ocorre, o sistema verifica qual é o número de IRQ ativo e chama a função 
 * gpioIsrHandler para tratar a interrupção correspondente. Após processar a interrupção, 
 * o IRQ é reconhecido, permitindo que o sistema lide com futuras interrupções.
 * =====================================================================================
 */
void ISR_Handler(void){
	/* Verify active IRQ number */
	unsigned int irq_number = HWREG(INTC_SIR_IRQ) & 0x7f; // 110 0010 & 111 1111 = 11
	/*armazena o número da interrupção ativa. Esse registrador contém informações sobre 
	qual interrupção está atualmente solicitando atenção.
	Isola e extrai o número da interrupção ativa, garantindo que apenas os bits relevantes 
	(7 bits mais baixos) sejam considerados. Isso é importante porque o sistema precisa 
	identificar qual interrupção específica está ativa para tratá-la corretamente.
*/
	switch (irq_number)
	{
	case 95:
		timerIrqHandler();
		break;
	case 98:
		gpioIsrHandlerA();
		putString("button A press! frequence\n\r",27);
	break;
	case 99:
		gpioIsrHandlerB();
	break;
	default:
		break;
	}
    
	/* acknowledge IRQ */
	HWREG(INTC_CONTROL) = 0x1;/*Reconhecer a interrupção e permitir que novas interrupções 
	sejam processadas. Página 557, manual*/
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:
    Configurações Iniciais: Configura GPIO, timer, LEDs, botões, e desativa o Watchdog Timer. 
	Envia uma mensagem pela UART indicando que a interrupção do GPIO está ativa.

    Estado Inicial dos LEDs: Todos os LEDs são desligados.

    Loop Principal:
        Dependendo do valor de flag_gpio12 (que é modificado pelas interrupções dos botões), 
		diferentes sequências de LEDs são ativadas e desativadas, com delays controlados por
		frequence.
        Se flag_gpio12 for 1, apenas os pinos 1 a 4 são ativados em sequência.
        Se flag_gpio12 for 2, apenas os pinos 5 a 7 são ativados em sequência.
        Caso contrário, todos os LEDs piscam simultaneamente.
 * =====================================================================================
 */
int main(void){
	
	/* Hardware setup */
	gpioSetup(); /*configura o modulo*/
	timerSetup(); /*configura o timer com interrupção*/
	ledConfig(); /*configura os leds*/
	butConfig(); /*configura os botões*/
	disableWdt(); /*desativa o  watdog*/

	/*para o terminal para indicar que o sistema está pronto para 
	lidar com interrupções GPIO. */

	ledOff(PIN1); /*inicia com os leds desligados*/
	ledOff(PIN2);
	ledOff(PIN3);

	while(true){ /*botão 12 muda o flag_gpio12
				o botão 14 muda a frequencia*/
		if(flag_gpio12 == 1){ /* Verifica se a variável flag_gpio12 
		é igual a 1. Se for, executa o bloco de código abaixo. */

	//putString("flag = 1\n\r",10); /* Envia uma string */
			ledOn(PIN1);
			delay(frequence);
			ledOn(PIN2);
			delay(frequence);
			ledOn(PIN3);
			delay(frequence);

			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);

			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);

			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
		}else if(flag_gpio12 == 2){
			/*Dependendo do valor de flag_gpio12 (0, 1, ou 2), a main alterna o 
			padrão de piscar dos LEDs. Com flag_gpio12 = 1, LEDs PIN1 a PIN4 piscam 
			em um padrão; com flag_gpio12 = 2, LEDs PIN5 a PIN7 piscam em um padrão 
			diferente; e com flag_gpio12 em qualquer outro valor, todos os LEDs piscam 
			juntos.

			frequence e Taxa de Piscamento: A variável frequence ajusta o intervalo de 
			piscamento dos LEDs. A pressão do botão B altera a frequência, mudando a taxa 
			de piscamento dos LED*/
			//putString("flag = 2\n\r",10); /* Envia uma string */

			ledOn(PIN1);
			delay(frequence);
			ledOn(PIN2);
			delay(frequence);

			ledOff(PIN1);
			ledOff(PIN2);

			ledOn(PIN1);
			ledOn(PIN2);


			ledOff(PIN1);
			ledOff(PIN2);
		}else{ /* Se flag_gpio12 não for 1 nem 2, executa o bloco de código abaixo. */
			ledOn(PIN1);
			ledOn(PIN2);
			ledOn(PIN3);
			delay(frequence);

			ledOff(PIN1);
			ledOff(PIN2);
			ledOff(PIN3);
			delay(frequence);
		}
	}

	return(0);
}
