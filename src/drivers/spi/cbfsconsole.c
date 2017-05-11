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

#define LINE_BUFFER_SIZE 0x100

static const struct region_device *g_rdev CAR_GLOBAL;
static uint8_t g_line_buffer[LINE_BUFFER_SIZE] CAR_GLOBAL;
static size_t g_cbfs_offset CAR_GLOBAL;
static size_t g_cbfs_size CAR_GLOBAL;
static uint32_t g_offset CAR_GLOBAL;

void cbfsconsole_init(void)
{
	struct cbfsf file;
	struct region_device cbfs_region;
	uint8_t *line_buffer = car_get_var_ptr(g_line_buffer);

	car_set_var(g_rdev, NULL);
	car_set_var(g_offset, 0);
	memset(line_buffer, 0, sizeof(g_line_buffer));

	cbfs_prepare_program_locate();
	if (cbfs_boot_locate(&file, "console", NULL) == 0) {
		cbfs_file_data(&cbfs_region, &file);
		car_set_var(g_cbfs_offset, region_device_offset(&cbfs_region));
		car_set_var(g_cbfs_size, region_device_sz(&cbfs_region));

		boot_device_init();
		car_set_var(g_rdev, boot_device_rw());
		rdev_eraseat(car_get_var(g_rdev), car_get_var(g_cbfs_offset),
			car_get_var(g_cbfs_size));
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
	}

}
