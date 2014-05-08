#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/bios_6_32_05_54/packages;C:/ti/ipc_1_23_05_40/packages
override XDCROOT = C:/ti/xdctools_3_22_04_46
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/bios_6_32_05_54/packages;C:/ti/ipc_1_23_05_40/packages;C:/ti/xdctools_3_22_04_46/packages;..
HOSTOS = Windows
endif
