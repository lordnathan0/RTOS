## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,28FP linker.cmd \
  package/cfg/app_p28FP.o28FP \

linker.cmd: package/cfg/app_p28FP.xdl
	$(SED) 's"^\"\(package/cfg/app_p28FPcfg.cmd\)\"$""\"C:/Users/aeozy_000/workspace_v5_1/Full_RTOS_system_TEST/.config/xconfig_app/\1\""' package/cfg/app_p28FP.xdl > $@
