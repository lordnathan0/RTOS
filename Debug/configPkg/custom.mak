## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,28FP linker.cmd \
  package/cfg/app_p28FP.o28FP \

linker.cmd: package/cfg/app_p28FP.xdl
	$(SED) 's"^\"\(package/cfg/app_p28FPcfg.cmd\)\"$""\"C:/Users/LordLaptop/BEMRT_PROJECTS/Full_RTOS_system_barebones/Debug/configPkg/\1\""' package/cfg/app_p28FP.xdl > $@
