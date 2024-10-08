#include "clock_module.h"
#include "hw_types.h"
#include "soc_AM335x.h"

void ckmSetCLKModuleRegister(CKM_MODULE_REG module, unsigned int value){
    HWREG(SOC_CM_PER_REGS + module) |= value;

}
unsigned int ckmGetCLKModuleRegister(CKM_MODULE_REG module){
    return (HWREG(SOC_CM_PER_REGS + module));
}