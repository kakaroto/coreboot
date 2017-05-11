/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef CONSOLE_CBFS_H
#define CONSOLE_CBFS_H 1

#include <rules.h>
#include <stdint.h>

void cbfsconsole_init(void);
void cbfsconsole_tx_byte(unsigned char c);

#define __CONSOLE_CBFS_ENABLE__	(CONFIG_CONSOLE_CBFS && ENV_RAMSTAGE)

#if __CONSOLE_CBFS_ENABLE__
static inline void __cbfsconsole_init(void)	{ cbfsconsole_init(); }
static inline void __cbfsconsole_tx_byte(u8 data)
{
	cbfsconsole_tx_byte(data);
}
#else
static inline void __cbfsconsole_init(void)	{}
static inline void __cbfsconsole_tx_byte(u8 data)	{}
#endif /* __CONSOLE_CBFS_ENABLE__ */


#endif /* CONSOLE_CBFS_H */
