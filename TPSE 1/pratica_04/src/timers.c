#include "timers.h"
#include "clock_module.h"

void DMTimerWaitForWrite(unsigned int reg ,Timer timer){
    unsigned int val_temp;

    switch (timer)
    {
    case TIMER2:
        val_temp = SOC_DMTIMER_2_REGS;
        break;
    case TIMER3:
        val_temp = SOC_DMTIMER_3_REGS;
        break;
    case TIMER4:
        val_temp = SOC_DMTIMER_4_REGS;
        break;
    case TIMER5:
        val_temp = SOC_DMTIMER_5_REGS;
        break;
    case TIMER6:
        val_temp = SOC_DMTIMER_6_REGS;
        break;
    case TIMER7 :
        val_temp = SOC_DMTIMER_7_REGS;
        break;
    default:
        return;
    }

    if (HWREG(val_temp + DMTIMER_TSICR) & 0x4)
    {
        while(reg & HWREG(val_temp + DMTIMER_TWPS));
    }
    
}

void timerEnable(Timer timer){
    DMTimerWaitForWrite(0x1, timer);

    HWREG(SOC_DMTIMER_BASE(timer) + DMTIMER_TCLR) |= 0x1;
}

void timerDisable(Timer timer){
    /* Wait for previous write to complete in TCLR */
    DMTimerWaitForWrite(0x1, timer);

    /* Stop the timer */
    HWREG(SOC_DMTIMER_BASE(timer) + DMTIMER_TCLR) &= ~(0x1);
}

void delay(unsigned int mSec,Timer timer){
    while (mSec != 0)
    {
        DMTimerWaitForWrite(0x2, timer);/*Garante que qualquer escrita anterior no registrador do 
		temporizador (0x2) tenha sido concluída. 0x2 - tcrr (0verflow)*/

        HWREG(SOC_DMTIMER_BASE(timer) + DMTIMER_TCRR) = 0x0;/*Configura o valor inicial do registrador do temporizador 
		para 0. Página 4462, manual*/

        timerEnable(timer);/*inicia o temporizador*/

        while(HWREG(SOC_DMTIMER_BASE(timer) + DMTIMER_TCRR) < TIMER_1MS_COUNT);/*Espera até que o contador do 
		temporizador atinja o valor correspondente a 1 milissegundo.*/

        timerDisable(timer);/*Para o temporizador(congela), página 4461, 
		manual*/

        mSec--;/*Reduz o número de milissegundos restantes.*/
    }
    
}
void timerSetup(Timer timer){
    unsigned int val_temp;
        switch (timer)
    {
    case TIMER2:
        val_temp = CKM_PER_TIMER2_CLKCTRL;
        break;
    case TIMER3:
        val_temp = CKM_PER_TIMER3_CLKCTRL;
        break;
    case TIMER4:
        val_temp = CKM_PER_TIMER4_CLKCTRL;
        break;
    case TIMER5:
        val_temp = CKM_PER_TIMER5_CLKCTRL;
        break;
    case TIMER6:
        val_temp = CKM_PER_TIMER6_CLKCTRL;
        break;
    case TIMER7 :
        val_temp = CKM_PER_TIMER7_CLKCTRL;
        break;
    default:
        return;
    }

    HWREG(val_temp) |= 0x2;

    while((HWREG(val_temp) & 0x3) != 0x2);
}