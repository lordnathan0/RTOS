vpath % C:/ti/ipc_1_23_05_40/packages/ti/sdo/
vpath %.c C:\ti\xdctools_3_22_04_46/packages/

CCOPTS ?= -v28 -DLARGE_MODEL=1 -ml --float_support=fpu32 -q -mo --program_level_compile -o3 -g --optimize_with_debug

XDC_ROOT = C:\ti\xdctools_3_22_04_46/packages/

IPC_ROOT = C:/ti/ipc_1_23_05_40/packages/ti/sdo/

BUILD_DEFS =  -Dxdc_runtime_Log_DISABLE_ALL

BIOS_INC  = -I"C:\ti\bios_6_32_05_54\packages"

SYSBIOS_INC  = -I"C:\ti\bios_6_32_05_54\packages"

IPC_INC    = -I"C:\ti\ipc_1_23_05_40\packages"

IO_INC    = -I"C:\ti\ipc_1_23_05_40\packages"

UTILS_INC    = -I"C:\ti\ipc_1_23_05_40\packages"

CC = C:/ti/ccsv5/tools/compiler/c2000/bin/cl2000 -c $(CCOPTS) -I C:/ti/ccsv5/tools/compiler/c2000/include

ASM = C:/ti/ccsv5/tools/compiler/c2000/bin/cl2000 -c $(CCOPTS) -I C:/ti/ccsv5/tools/compiler/c2000/include

AR = C:/ti/ccsv5/tools/compiler/c2000/bin/ar2000 rq

DEL = cmd.exe /q /c del /f

define RM
	$(if $(wildcard $@),$(DEL) $@,:)
endef

define ASSEMBLE
	@echo asm28FP $< ...
	@$(ASM) $(BUILD_DEFS) -I "C:\ti\xdctools_3_22_04_46/packages" -Dxdc_target_types__=ti/targets/std.h -Dxdc_target_name__=C28_float $(IPC_INC) $<
endef


all: io.lib


IO.obj: io/IO.c utils/NameServer.c utils/MultiProc.c utils/List.c io/DriverTable.c io/Stream.c io/converters/DriverAdapter.c makefile
	@-$(RM) IO.obj
	@echo cl28FP $< ...
	@$(CC) $(BUILD_DEFS) -I "C:\ti\xdctools_3_22_04_46/packages" -Dxdc_target_types__=ti/targets/std.h -Dxdc_target_name__=C28_float $(SYSBIOS_INC) $(BIOS_INC) $(IO_INC) \
	$(IPC_INC) $(UTILS_INC) \
	$(IPC_ROOT)io/IO.c \
	$(IPC_ROOT)utils/NameServer.c \
	$(IPC_ROOT)utils/MultiProc.c \
	$(IPC_ROOT)utils/List.c \
	$(IPC_ROOT)io/DriverTable.c \
	$(IPC_ROOT)io/Stream.c \
	$(IPC_ROOT)io/converters/DriverAdapter.c \

io.lib: IO.obj 
	@echo ar28FP $^ ...
	@-$(RM) io.lib
	@$(AR) $@ $^

clean:
	-$(RM) io.lib
	-$(RM) IO.obj
