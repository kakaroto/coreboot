ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/cpu.c
bootblock-y += bootblock/pch.c
bootblock-y += pmutil.c
bootblock-y += bootblock/report_platform.c
bootblock-y += gpio.c
bootblock-y += gspi.c
bootblock-y += memmap.c
bootblock-y += spi.c
bootblock-$(CONFIG_UART_DEBUG) += uart.c

romstage-y += gpio.c
romstage-y += gspi.c
romstage-y += memmap.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c
romstage-$(CONFIG_UART_DEBUG) += uart.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += finalize.c
ramstage-y += gpio.c
ramstage-y += graphics.c
ramstage-y += gspi.c
ramstage-y += gpio.c
ramstage-y += lpc.c
ramstage-y += memmap.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-$(CONFIG_PLATFORM_USES_FSP2_0) += reset.c
ramstage-y += smmrelocate.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-$(CONFIG_UART_DEBUG) += uart.c
ramstage-$(CONFIG_UART_DEBUG) += uart_pch.c
ramstage-y += vr_config.c
ramstage-y += sd.c

smm-y += gpio.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-$(CONFIG_SPI_FLASH_SMM) += spi.c
smm-$(CONFIG_UART_DEBUG) += uart.c
smm-$(CONFIG_UART_DEBUG) += uart_pch.c

postcar-y += memmap.c
postcar-y += pmutil.c
postcar-y += spi.c
postcar-$(CONFIG_UART_DEBUG) += uart.c

verstage-y += gspi.c
verstage-y += pmutil.c
verstage-y += spi.c
verstage-$(CONFIG_UART_DEBUG) += uart.c

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include/fsp20
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp2_0/cannonlake

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake
CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include

endif
