/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH
 *  written by Stefan Reinauer <stepan@coresystems.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "inteltool.h"

#define RCBA8(x)	*((volatile u8 *)(rcba + x))
#define RCBA16(x)	*((volatile u16 *)(rcba + x))
#define RCBA32(x)	*((volatile u32 *)(rcba + x))

#define IOBP_RETRY 1000

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_READY	0x0001
#define  IOBPS_TX_MASK	0x0006
#define  IOBPS_MASK     0xff00
#define  IOBPS_READ     0x0600
#define  IOBPS_WRITE	0x0700
#define IOBPU		0x233a
#define  IOBPU_MAGIC	0xf000
#define  IOBP_PCICFG_READ	0x0400
#define  IOBP_PCICFG_WRITE	0x0500

/* SATA IOBP Registers */
#define SATA_IOBP_SP0_SECRT88	0xea002688
#define SATA_IOBP_SP1_SECRT88	0xea002488
#define SATA_IOBP_SP2_SECRT88	0xea002288
#define SATA_IOBP_SP3_SECRT88	0xea002088

#define SATA_SECRT88_VADJ_MASK	0xff
#define SATA_SECRT88_VADJ_SHIFT	16

#define SATA_IOBP_SP0DTLE_DATA	0xea002750
#define SATA_IOBP_SP0DTLE_EDGE	0xea002754
#define SATA_IOBP_SP1DTLE_DATA	0xea002550
#define SATA_IOBP_SP1DTLE_EDGE	0xea002554
#define SATA_IOBP_SP2DTLE_DATA	0xea002350
#define SATA_IOBP_SP2DTLE_EDGE	0xea002354
#define SATA_IOBP_SP3DTLE_DATA	0xea002150
#define SATA_IOBP_SP3DTLE_EDGE	0xea002154

#define SATA_DTLE_MASK		0xF
#define SATA_DTLE_DATA_SHIFT	24
#define SATA_DTLE_EDGE_SHIFT	16

static inline int iobp_poll(volatile uint8_t *rcba)
{
	unsigned int try;

	for (try = IOBP_RETRY; try > 0; try--) {
		u16 status = RCBA16(IOBPS);
		if ((status & IOBPS_READY) == 0)
			return 1;
		usleep(10);
	}

	printf ("IOBP: timeout waiting for transaction to complete\n");
	return 0;
}

u32 pch_iobp_read(volatile uint8_t *rcba, u32 address)
{
	u16 status;

	if (!iobp_poll(rcba))
		return 0;

	/* Set the address */
	RCBA32(IOBPIRI) = address;

	/* READ OPCODE */
	status = RCBA16(IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_READ;
	RCBA16(IOBPS) = status;

	/* Undocumented magic */
	RCBA16(IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(IOBPS);
	status |= IOBPS_READY;
	RCBA16(IOBPS) = status;

	if (!iobp_poll(rcba))
		return 0;

	/* Check for successful transaction */
	status = RCBA16(IOBPS);
	if (status & IOBPS_TX_MASK) {
		printf( "IOBP: read 0x%08x failed\n", address);
		return 0;
	}

	/* Read IOBP data */
	return RCBA32(IOBPD);
}

void print_iobp(volatile uint8_t *rcba)
{
  uint32_t secrt, data, edge;
  printf("\n============= IOBP ==============\n\n");

  secrt = pch_iobp_read (rcba, SATA_IOBP_SP0_SECRT88);
  data = pch_iobp_read (rcba, SATA_IOBP_SP0DTLE_DATA);
  edge = pch_iobp_read (rcba, SATA_IOBP_SP0DTLE_EDGE);
  printf ("SP0 Secret : %X\n", secrt);
  printf ("SP0 DTLE Data : %X\n", data);
  printf ("SP0 DTLE Edge : %X\n", edge);
  printf ("SATA Port 0 TX : %X\n",
      (secrt >> SATA_SECRT88_VADJ_SHIFT) & SATA_SECRT88_VADJ_MASK);
  printf ("SATA Port 0 DTLE : %X - %X\n",
      (data >> SATA_DTLE_DATA_SHIFT) & SATA_DTLE_MASK,
      (edge >> SATA_DTLE_EDGE_SHIFT) & SATA_DTLE_MASK);

  secrt = pch_iobp_read (rcba, SATA_IOBP_SP1_SECRT88);
  data = pch_iobp_read (rcba, SATA_IOBP_SP1DTLE_DATA);
  edge = pch_iobp_read (rcba, SATA_IOBP_SP1DTLE_EDGE);
  printf ("SP1 Secret : %X\n", secrt);
  printf ("SP1 DTLE Data : %X\n", data);
  printf ("SP1 DTLE Edge : %X\n", edge);
  printf ("SATA Port 1 TX : %X\n",
      (secrt >> SATA_SECRT88_VADJ_SHIFT) & SATA_SECRT88_VADJ_MASK);
  printf ("SATA Port 1 DTLE : %X - %X\n",
      (data >> SATA_DTLE_DATA_SHIFT) & SATA_DTLE_MASK,
      (edge >> SATA_DTLE_EDGE_SHIFT) & SATA_DTLE_MASK);

  secrt = pch_iobp_read (rcba, SATA_IOBP_SP2_SECRT88);
  data = pch_iobp_read (rcba, SATA_IOBP_SP2DTLE_DATA);
  edge = pch_iobp_read (rcba, SATA_IOBP_SP2DTLE_EDGE);
  printf ("SP2 Secret : %X\n", secrt);
  printf ("SP2 DTLE Data : %X\n", data);
  printf ("SP2 DTLE Edge : %X\n", edge);
  printf ("SATA Port 2 TX : %X\n",
      (secrt >> SATA_SECRT88_VADJ_SHIFT) & SATA_SECRT88_VADJ_MASK);
  printf ("SATA Port 2 DTLE : %X - %X\n",
      (data >> SATA_DTLE_DATA_SHIFT) & SATA_DTLE_MASK,
      (edge >> SATA_DTLE_EDGE_SHIFT) & SATA_DTLE_MASK);

  secrt = pch_iobp_read (rcba, SATA_IOBP_SP3_SECRT88);
  data = pch_iobp_read (rcba, SATA_IOBP_SP3DTLE_DATA);
  edge = pch_iobp_read (rcba, SATA_IOBP_SP3DTLE_EDGE);
  printf ("SP3 Secret : %X\n", secrt);
  printf ("SP3 DTLE Data : %X\n", data);
  printf ("SP3 DTLE Edge : %X\n", edge);
  printf ("SATA Port 3 TX : %X\n",
      (secrt >> SATA_SECRT88_VADJ_SHIFT) & SATA_SECRT88_VADJ_MASK);
  printf ("SATA Port 3 DTLE : %X - %X\n",
      (data >> SATA_DTLE_DATA_SHIFT) & SATA_DTLE_MASK,
      (edge >> SATA_DTLE_EDGE_SHIFT) & SATA_DTLE_MASK);

  unsigned int i;
  for (i = 0xea002000; i < 0xea002800; i += 4) {
    uint32_t val = pch_iobp_read (rcba, i);
    if (val)
      printf("0x%04x: 0x%08x\n", i, val);
  }
}

int print_rcba(struct pci_dev *sb)
{
	int i, size = 0x4000;
	volatile uint8_t *rcba;
	uint32_t rcba_phys;

	printf("\n============= RCBA ==============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH6:
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
	case PCI_DEVICE_ID_INTEL_ICH8:
	case PCI_DEVICE_ID_INTEL_ICH8M:
	case PCI_DEVICE_ID_INTEL_ICH8ME:
	case PCI_DEVICE_ID_INTEL_ICH9DH:
	case PCI_DEVICE_ID_INTEL_ICH9DO:
	case PCI_DEVICE_ID_INTEL_ICH9R:
	case PCI_DEVICE_ID_INTEL_ICH9:
	case PCI_DEVICE_ID_INTEL_ICH9M:
	case PCI_DEVICE_ID_INTEL_ICH9ME:
	case PCI_DEVICE_ID_INTEL_ICH10:
	case PCI_DEVICE_ID_INTEL_ICH10R:
	case PCI_DEVICE_ID_INTEL_NM10:
	case PCI_DEVICE_ID_INTEL_I63XX:
	case PCI_DEVICE_ID_INTEL_3400:
	case PCI_DEVICE_ID_INTEL_3420:
	case PCI_DEVICE_ID_INTEL_3450:
	case PCI_DEVICE_ID_INTEL_3400_DESKTOP:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF:
	case PCI_DEVICE_ID_INTEL_B55_A:
	case PCI_DEVICE_ID_INTEL_B55_B:
	case PCI_DEVICE_ID_INTEL_H55:
	case PCI_DEVICE_ID_INTEL_H57:
	case PCI_DEVICE_ID_INTEL_HM55:
	case PCI_DEVICE_ID_INTEL_HM57:
	case PCI_DEVICE_ID_INTEL_P55:
	case PCI_DEVICE_ID_INTEL_PM55:
	case PCI_DEVICE_ID_INTEL_Q57:
	case PCI_DEVICE_ID_INTEL_QM57:
	case PCI_DEVICE_ID_INTEL_QS57:
	case PCI_DEVICE_ID_INTEL_Z68:
	case PCI_DEVICE_ID_INTEL_P67:
	case PCI_DEVICE_ID_INTEL_UM67:
	case PCI_DEVICE_ID_INTEL_HM65:
	case PCI_DEVICE_ID_INTEL_H67:
	case PCI_DEVICE_ID_INTEL_HM67:
	case PCI_DEVICE_ID_INTEL_Q65:
	case PCI_DEVICE_ID_INTEL_QS67:
	case PCI_DEVICE_ID_INTEL_Q67:
	case PCI_DEVICE_ID_INTEL_QM67:
	case PCI_DEVICE_ID_INTEL_B65:
	case PCI_DEVICE_ID_INTEL_C202:
	case PCI_DEVICE_ID_INTEL_C204:
	case PCI_DEVICE_ID_INTEL_C206:
	case PCI_DEVICE_ID_INTEL_H61:
	case PCI_DEVICE_ID_INTEL_Z77:
	case PCI_DEVICE_ID_INTEL_Z75:
	case PCI_DEVICE_ID_INTEL_Q77:
	case PCI_DEVICE_ID_INTEL_Q75:
	case PCI_DEVICE_ID_INTEL_B75:
	case PCI_DEVICE_ID_INTEL_H77:
	case PCI_DEVICE_ID_INTEL_C216:
	case PCI_DEVICE_ID_INTEL_QM77:
	case PCI_DEVICE_ID_INTEL_QS77:
	case PCI_DEVICE_ID_INTEL_HM77:
	case PCI_DEVICE_ID_INTEL_UM77:
	case PCI_DEVICE_ID_INTEL_HM76:
	case PCI_DEVICE_ID_INTEL_HM75:
	case PCI_DEVICE_ID_INTEL_HM70:
	case PCI_DEVICE_ID_INTEL_NM70:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP:
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
	case PCI_DEVICE_ID_INTEL_ICH2:
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
	case PCI_DEVICE_ID_INTEL_ICH5:
		printf("This southbridge does not have RCBA.\n");
		return 1;
	default:
		printf("Error: Dumping RCBA on this southbridge is not (yet) supported.\n");
		return 1;
	}

	rcba = map_physical(rcba_phys, size);

	if (rcba == NULL) {
		perror("Error mapping RCBA");
		exit(1);
	}

	printf("RCBA = 0x%08x (MEM)\n\n", rcba_phys);

	for (i = 0; i < size; i += 4) {
		if (*(uint32_t *)(rcba + i))
			printf("0x%04x: 0x%08x\n", i, *(uint32_t *)(rcba + i));
	}

        print_iobp (rcba);

	unmap_physical((void *)rcba, size);
	return 0;
}
