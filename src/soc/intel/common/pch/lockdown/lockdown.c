/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/chip.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/pcr.h>
#include <intelpch/lockdown.h>
#include <soc/pci_devs.h>
#include <soc/pcr_ids.h>
#include <soc/soc_chip.h>
#include <string.h>

#define PCR_DMI_GCS		0x274C
#define PCR_DMI_GCS_BILD	(1 << 0)

static void dmi_lockdown_cfg(void)
{
	/*
	 * GCS reg of DMI
	 *
	 * When set, prevents GCS.BBS from being changed
	 * GCS.BBS: (Boot BIOS Strap) This field determines the destination
	 * of accesses to the BIOS memory range.
	 *	Bits Description
	 *	"0b": SPI
	 *	"1b": LPC/eSPI
	 */
	pcr_or8(PID_DMI, PCR_DMI_GCS, PCR_DMI_GCS_BILD);
}

static void fast_spi_lockdown_cfg(void)
{
	if (!IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_FAST_SPI))
		return;

	/* Set FAST_SPI opcode menu */
	fast_spi_set_opcode_menu();

	/* Lock FAST_SPIBAR */
	fast_spi_lock_bar();

	/* Bios Interface Lock */
	fast_spi_set_bios_interface_lock_down();

	/* Bios Lock */
	fast_spi_set_lock_enable();
}

/*
 * platform_lockdown_config has 2 major part.
 * 1. Common SoC lockdown configuration.
 * 2. SoC specific lockdown configuration as per Silicon
 * guideline.
 */
static void platform_lockdown_config(void *unused)
{

	/* SPI lock down configuration */
	fast_spi_lockdown_cfg();

	/* DMI lock down configuration */
	dmi_lockdown_cfg();

	/* SoC lock down configuration */
	soc_lockdown_config();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, platform_lockdown_config,
				NULL);
