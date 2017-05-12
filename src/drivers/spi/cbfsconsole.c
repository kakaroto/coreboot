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

#include <arch/early_variables.h>
#include <region.h>
#include <boot_device.h>
#include <cbfs.h>
#include <console/cbfs.h>
#include <string.h>
#include <assert.h>
#include <spi_flash.h>

#define LINE_BUFFER_SIZE 0x1000

static const struct region_device *g_rdev CAR_GLOBAL;
static uint8_t g_line_buffer[LINE_BUFFER_SIZE] CAR_GLOBAL;
static size_t g_cbfs_offset CAR_GLOBAL;
static size_t g_cbfs_size CAR_GLOBAL;
static uint32_t g_offset CAR_GLOBAL;

void cbfsconsole_init(void)
{
	struct cbfsf file;

	car_set_var(g_rdev, NULL);
	car_set_var(g_offset, 0);

	cbfs_prepare_program_locate();
	if (cbfs_boot_locate(&file, "console", NULL) == 0) {
		struct region_device cbfs_region;
		const struct region_device *rdev;
		uint8_t *line_buffer = car_get_var_ptr(g_line_buffer);
		uint32_t cbfs_offset;
		uint32_t cbfs_size;
		uint32_t offset = 0;
		int i;
		int len = LINE_BUFFER_SIZE;

		cbfs_file_data(&cbfs_region, &file);
		cbfs_offset = region_device_offset(&cbfs_region);
		cbfs_size = region_device_sz(&cbfs_region);

		boot_device_init();
		rdev = boot_device_rw();

		/*
		 * We need to check the region until we find a 0xff indicating
		 * the end of a previous log write.
		 * We can't erase the region because one stage would erase the
		 * data from the previous stage. Also, it looks like doing an
		 * erase could completely freeze the SPI controller and then
		 * we can't write anything anymore.
		 */
		for (i = 0; i < len && offset < cbfs_size; ) {
			// Fill the buffer on first iteration
			if (i == 0) {
				len = min(LINE_BUFFER_SIZE, cbfs_size - offset);
				rdev_readat(rdev, line_buffer,
					cbfs_offset + offset, len);
			}
			if (line_buffer[i] == 0xff) {
				offset += i;
				break;
			}
			// If we're done, repeat the process for the next sector
			if (++i == LINE_BUFFER_SIZE) {
				offset += len;
				i = 0;
			}
		}
		// Make sure there is still space left on the console
		if (offset < cbfs_size) {
			// Now we can enable tx_byte
			car_set_var(g_cbfs_offset, cbfs_offset);
			car_set_var(g_cbfs_size, cbfs_size);
			car_set_var(g_offset, offset);
			memset(line_buffer, 0, LINE_BUFFER_SIZE);
			car_set_var(g_rdev, rdev);
		} else {
			printk(BIOS_INFO, "No space left on 'console' region in CBFS.");
		}
	} else {
		printk(BIOS_INFO, "Can't find 'console' region in CBFS.");
	}

}

void cbfsconsole_tx_byte(unsigned char c)
{
	const struct region_device *rdev = car_get_var(g_rdev);

	if (rdev) {
		uint8_t *line_buffer = car_get_var_ptr(g_line_buffer);
		uint32_t offset = car_get_var(g_offset);
		uint32_t cbfs_size = car_get_var(g_cbfs_size);
		int i;
		uint32_t len = LINE_BUFFER_SIZE;

		/* Prevent any recursive loops in case the spi flash driver
		 * calls printk (in case of transaction timeout or
		 * any other error while writing) */
		car_set_var(g_rdev, NULL);

		for (i = 0; i < LINE_BUFFER_SIZE - 1; i++) {
			if (line_buffer[i] == 0) {
				line_buffer[i] = c;
				line_buffer[i+1] = 0;
				len = i+1;
				break;
			}
		}
		if (len >= LINE_BUFFER_SIZE - 1 ||
			offset + len >= cbfs_size || c == '\n') {
			uint32_t cbfs_offset = car_get_var(g_cbfs_offset);

			// We crop the rest of this line, but we're not
			// expected to go beyond the buffer size anyways
			if (offset + len >= cbfs_size)
				len = cbfs_size - offset;

			rdev_writeat(rdev, line_buffer, cbfs_offset + offset,
				len);

			offset += len;
			if (offset >= cbfs_size)
				offset = 0;
			car_set_var(g_offset, offset);
			line_buffer[0] = 0;
		}
		car_set_var(g_rdev, rdev);
	}

}
