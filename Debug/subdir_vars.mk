################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../TMS320F28335.cmd 

CFG_SRCS += \
../app.cfg 

ASM_SRCS += \
../DSP2833x_ADC_cal.asm \
../DSP2833x_CSMPasswords.asm \
../DSP2833x_CodeStartBranch.asm \
../DSP2833x_DBGIER.asm \
../DSP2833x_DisInt.asm \
../DSP2833x_usDelay.asm 

C_SRCS += \
../ALFAT.c \
../Action.c \
../BQ.c \
../Bluetooth.c \
../Charge.c \
../DSP2833x_Adc.c \
../DSP2833x_CpuTimers.c \
../DSP2833x_DMA.c \
../DSP2833x_DefaultIsr.c \
../DSP2833x_ECan.c \
../DSP2833x_ECap.c \
../DSP2833x_EPwm.c \
../DSP2833x_EQep.c \
../DSP2833x_GlobalVariableDefs.c \
../DSP2833x_Gpio.c \
../DSP2833x_I2C.c \
../DSP2833x_Mcbsp.c \
../DSP2833x_MemCopy.c \
../DSP2833x_PieCtrl.c \
../DSP2833x_PieVect.c \
../DSP2833x_Sci.c \
../DSP2833x_Spi.c \
../DSP2833x_SysCtrl.c \
../DSP2833x_Xintf.c \
../GPIO.c \
../I2C.c \
../LCD.c \
../Race.c \
../Sleep.c \
../adc.c \
../bq_pack.c \
../data_flash.c \
../eCAN.c \
../ext_adc.c \
../main.c \
../spi_if.c 

OBJS += \
./ALFAT.obj \
./Action.obj \
./BQ.obj \
./Bluetooth.obj \
./Charge.obj \
./DSP2833x_ADC_cal.obj \
./DSP2833x_Adc.obj \
./DSP2833x_CSMPasswords.obj \
./DSP2833x_CodeStartBranch.obj \
./DSP2833x_CpuTimers.obj \
./DSP2833x_DBGIER.obj \
./DSP2833x_DMA.obj \
./DSP2833x_DefaultIsr.obj \
./DSP2833x_DisInt.obj \
./DSP2833x_ECan.obj \
./DSP2833x_ECap.obj \
./DSP2833x_EPwm.obj \
./DSP2833x_EQep.obj \
./DSP2833x_GlobalVariableDefs.obj \
./DSP2833x_Gpio.obj \
./DSP2833x_I2C.obj \
./DSP2833x_Mcbsp.obj \
./DSP2833x_MemCopy.obj \
./DSP2833x_PieCtrl.obj \
./DSP2833x_PieVect.obj \
./DSP2833x_Sci.obj \
./DSP2833x_Spi.obj \
./DSP2833x_SysCtrl.obj \
./DSP2833x_Xintf.obj \
./DSP2833x_usDelay.obj \
./GPIO.obj \
./I2C.obj \
./LCD.obj \
./Race.obj \
./Sleep.obj \
./adc.obj \
./bq_pack.obj \
./data_flash.obj \
./eCAN.obj \
./ext_adc.obj \
./main.obj \
./spi_if.obj 

GEN_SRCS += \
./configPkg/compiler.opt \
./configPkg/linker.cmd 

ASM_DEPS += \
./DSP2833x_ADC_cal.pp \
./DSP2833x_CSMPasswords.pp \
./DSP2833x_CodeStartBranch.pp \
./DSP2833x_DBGIER.pp \
./DSP2833x_DisInt.pp \
./DSP2833x_usDelay.pp 

C_DEPS += \
./ALFAT.pp \
./Action.pp \
./BQ.pp \
./Bluetooth.pp \
./Charge.pp \
./DSP2833x_Adc.pp \
./DSP2833x_CpuTimers.pp \
./DSP2833x_DMA.pp \
./DSP2833x_DefaultIsr.pp \
./DSP2833x_ECan.pp \
./DSP2833x_ECap.pp \
./DSP2833x_EPwm.pp \
./DSP2833x_EQep.pp \
./DSP2833x_GlobalVariableDefs.pp \
./DSP2833x_Gpio.pp \
./DSP2833x_I2C.pp \
./DSP2833x_Mcbsp.pp \
./DSP2833x_MemCopy.pp \
./DSP2833x_PieCtrl.pp \
./DSP2833x_PieVect.pp \
./DSP2833x_Sci.pp \
./DSP2833x_Spi.pp \
./DSP2833x_SysCtrl.pp \
./DSP2833x_Xintf.pp \
./GPIO.pp \
./I2C.pp \
./LCD.pp \
./Race.pp \
./Sleep.pp \
./adc.pp \
./bq_pack.pp \
./data_flash.pp \
./eCAN.pp \
./ext_adc.pp \
./main.pp \
./spi_if.pp 

GEN_MISC_DIRS += \
./configPkg/ 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_OPTS += \
./configPkg/compiler.opt 

GEN_SRCS__QUOTED += \
"configPkg\compiler.opt" \
"configPkg\linker.cmd" 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

C_DEPS__QUOTED += \
"ALFAT.pp" \
"Action.pp" \
"BQ.pp" \
"Bluetooth.pp" \
"Charge.pp" \
"DSP2833x_Adc.pp" \
"DSP2833x_CpuTimers.pp" \
"DSP2833x_DMA.pp" \
"DSP2833x_DefaultIsr.pp" \
"DSP2833x_ECan.pp" \
"DSP2833x_ECap.pp" \
"DSP2833x_EPwm.pp" \
"DSP2833x_EQep.pp" \
"DSP2833x_GlobalVariableDefs.pp" \
"DSP2833x_Gpio.pp" \
"DSP2833x_I2C.pp" \
"DSP2833x_Mcbsp.pp" \
"DSP2833x_MemCopy.pp" \
"DSP2833x_PieCtrl.pp" \
"DSP2833x_PieVect.pp" \
"DSP2833x_Sci.pp" \
"DSP2833x_Spi.pp" \
"DSP2833x_SysCtrl.pp" \
"DSP2833x_Xintf.pp" \
"GPIO.pp" \
"I2C.pp" \
"LCD.pp" \
"Race.pp" \
"Sleep.pp" \
"adc.pp" \
"bq_pack.pp" \
"data_flash.pp" \
"eCAN.pp" \
"ext_adc.pp" \
"main.pp" \
"spi_if.pp" 

OBJS__QUOTED += \
"ALFAT.obj" \
"Action.obj" \
"BQ.obj" \
"Bluetooth.obj" \
"Charge.obj" \
"DSP2833x_ADC_cal.obj" \
"DSP2833x_Adc.obj" \
"DSP2833x_CSMPasswords.obj" \
"DSP2833x_CodeStartBranch.obj" \
"DSP2833x_CpuTimers.obj" \
"DSP2833x_DBGIER.obj" \
"DSP2833x_DMA.obj" \
"DSP2833x_DefaultIsr.obj" \
"DSP2833x_DisInt.obj" \
"DSP2833x_ECan.obj" \
"DSP2833x_ECap.obj" \
"DSP2833x_EPwm.obj" \
"DSP2833x_EQep.obj" \
"DSP2833x_GlobalVariableDefs.obj" \
"DSP2833x_Gpio.obj" \
"DSP2833x_I2C.obj" \
"DSP2833x_Mcbsp.obj" \
"DSP2833x_MemCopy.obj" \
"DSP2833x_PieCtrl.obj" \
"DSP2833x_PieVect.obj" \
"DSP2833x_Sci.obj" \
"DSP2833x_Spi.obj" \
"DSP2833x_SysCtrl.obj" \
"DSP2833x_Xintf.obj" \
"DSP2833x_usDelay.obj" \
"GPIO.obj" \
"I2C.obj" \
"LCD.obj" \
"Race.obj" \
"Sleep.obj" \
"adc.obj" \
"bq_pack.obj" \
"data_flash.obj" \
"eCAN.obj" \
"ext_adc.obj" \
"main.obj" \
"spi_if.obj" 

ASM_DEPS__QUOTED += \
"DSP2833x_ADC_cal.pp" \
"DSP2833x_CSMPasswords.pp" \
"DSP2833x_CodeStartBranch.pp" \
"DSP2833x_DBGIER.pp" \
"DSP2833x_DisInt.pp" \
"DSP2833x_usDelay.pp" 

C_SRCS__QUOTED += \
"../ALFAT.c" \
"../Action.c" \
"../BQ.c" \
"../Bluetooth.c" \
"../Charge.c" \
"../DSP2833x_Adc.c" \
"../DSP2833x_CpuTimers.c" \
"../DSP2833x_DMA.c" \
"../DSP2833x_DefaultIsr.c" \
"../DSP2833x_ECan.c" \
"../DSP2833x_ECap.c" \
"../DSP2833x_EPwm.c" \
"../DSP2833x_EQep.c" \
"../DSP2833x_GlobalVariableDefs.c" \
"../DSP2833x_Gpio.c" \
"../DSP2833x_I2C.c" \
"../DSP2833x_Mcbsp.c" \
"../DSP2833x_MemCopy.c" \
"../DSP2833x_PieCtrl.c" \
"../DSP2833x_PieVect.c" \
"../DSP2833x_Sci.c" \
"../DSP2833x_Spi.c" \
"../DSP2833x_SysCtrl.c" \
"../DSP2833x_Xintf.c" \
"../GPIO.c" \
"../I2C.c" \
"../LCD.c" \
"../Race.c" \
"../Sleep.c" \
"../adc.c" \
"../bq_pack.c" \
"../data_flash.c" \
"../eCAN.c" \
"../ext_adc.c" \
"../main.c" \
"../spi_if.c" 

ASM_SRCS__QUOTED += \
"../DSP2833x_ADC_cal.asm" \
"../DSP2833x_CSMPasswords.asm" \
"../DSP2833x_CodeStartBranch.asm" \
"../DSP2833x_DBGIER.asm" \
"../DSP2833x_DisInt.asm" \
"../DSP2833x_usDelay.asm" 

CFG_SRCS__QUOTED += \
"../app.cfg" 

GEN_OPTS__FLAG += \
--cmd_file="./configPkg/compiler.opt" 

GEN_CMDS__FLAG += \
-l"./configPkg/linker.cmd" 


