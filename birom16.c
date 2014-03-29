/*
Kuji16 Flash MCU Programmer
Copyright (C) 2014 Kari Sigurjonsson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
@addtogroup birom16
@{
*/
#include "stdafx.h"

int birom16_new(struct birom16_state **state, struct chipdef16 *chipdef, int clockid, char *com_path) {
	char path[256];
	int rc;

	assert(state);

	*state = calloc(1, sizeof(struct birom16_state));
	assert(*state);

	(*state)->chipdef = chipdef;
	(*state)->clockid = clockid;

	snprintf(path, sizeof(path) - 1, "%s:%d:8N1", com_path, chipdef->bps[clockid]);

	memset(&(*state)->serial, 0x00, sizeof(struct serial));
	rc = serial_open(&(*state)->serial, path);
	if (rc != E_NONE) {
		LOGE("Error opening serial port '%s'.", path);
		return E_OPEN;
	}

	serial_purge(&(*state)->serial);

	LOGD("User says MCU is '%s' on %s running at %dHz.", mcu16_name(chipdef->mcu), path, chipdef->clock[(*state)->clockid]);

	memset(path, 0x00, sizeof(path));
	snprintf(path, sizeof(path) - 1, "kernal16/%s", chipdef->kernal);

	(*state)->kernalsize = filedata(path, &(*state)->kernaldata);
	if ((*state)->kernalsize <= 0 || (*state)->kernaldata == NULL) {
		LOGE("Could not read contents of '%s'.", path);
		return E_READ;
	}

	LOGD("Loaded '%s' (0x%02X bytes).", path, (*state)->kernalsize);

	return E_NONE;
}

void birom16_free(struct birom16_state **state) {
	if (state && *state) {
		if ((*state)->kernaldata) {
			free((*state)->kernaldata);
		}
		serial_close(&(*state)->serial);
		free(*state);
		*state = NULL;
	}
}

int birom16_connect(struct birom16_state *state) {
	int rc;
	uint8_t buf[2];

	serial_purge(&state->serial);

	memset(&buf, 0x00, sizeof(buf));

	double timeout = get_ticks() + 10;
	while (get_ticks() < timeout) {
		msleep(10);

		buf[0] = BIROM16_CMD_PROBE;
		rc = serial_write(&state->serial, buf, 1);
		if (rc < 1) {
			LOGE("Error writing to serial port! Aborting.");
			return rc;
		}

		serial_drain(&state->serial);

		memset(&buf, 0x00, sizeof(buf));
		while ((rc = serial_read(&state->serial, buf, 1)) > 0) {
			if (buf[0] == BIROM16_RESP_PROBE) {
				return E_NONE;	//Returning zero to mean MCU is alive.
			}
		}

		if (rc < 0) {
			LOGE("Error reading from serial port! Aborting.");
			return rc;
		}

		if (buf[0] > 0 && buf[0] != BIROM16_RESP_PROBE) {
			LOGE("Malformed response from MCU (0x%X). Please power off board and try again.", buf[0]);
			return E_MSGMALFORMED;
		}
	}

	LOGD("ERROR - Time-out waiting for MCU.");
	return E_TIMEOUT;
}

int birom16_write(struct birom16_state *state, uint16_t address, uint8_t *data, uint16_t size) {
	uint8_t buffer[4096];
	memset(buffer, 0x00, sizeof(buffer));

	serial_purge(&state->serial);

	buffer[0] = BIROM16_CMD_WRITE;
	buffer[1] = (address & 0xFF00) >> 8;
	buffer[2] = (address & 0x00FF);
	buffer[3] = (size & 0xFF00) >> 8;
	buffer[4] = (size & 0x00FF);

	memcpy(buffer + 5, data, size);
	size += 5;

	uint8_t csum = checksum(buffer, size);
	buffer[size++] = csum;

	LOGD("Writing %d bytes to address 0x%04X...", size, address);

	int rc = serial_write(&state->serial, buffer, size);
	if (rc < size) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	//Give MCU some breathing space.
	serial_drain(&state->serial);

	//Expect single byte result of operation from MCU.
	uint8_t code = 0;
	uint8_t value = 0;
	rc = serial_read(&state->serial, &code, 1);

	if (rc <= 0) {
		LOGE("MCU did not respond.");
		return E_READ;
	}

	//Split byte into sepearate nibble values.
	value = (code & 0x0F);	//Low nibble is response.
	code &= 0xF0;			//High nibble is mirror of command codes high nibble.

	if (code != (BIROM16_CMD_WRITE & 0xF0) || value != BIROM16_RESP_ACK) {
		LOGE("Invalid MCU response: Code %d, value %d.", code, value);
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

int birom16_call(struct birom16_state *state, uint16_t address) {
	uint8_t buffer[5];

	memset(buffer, 0x00, sizeof(buffer));

	buffer[0] = BIROM16_CMD_CALL;
	buffer[1] = (address & 0xFF000000) >> 24;
	buffer[2] = (address & 0x00FF0000) >> 16;
	buffer[3] = (address & 0x0000FF00) >> 8;
	buffer[4] = (address & 0x000000FF);

	serial_purge(&state->serial);

	LOGD("Calling stage 2 at address 0x%04X.", address);

	unsigned int rc = serial_write(&state->serial, buffer, sizeof(buffer));
	if (rc < sizeof(buffer)) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	serial_drain(&state->serial);

	//Expect single byte result of operation from MCU.
	rc = serial_read(&state->serial, buffer, sizeof(buffer) - 1);
	if (buffer[0] != 0x31) {
		LOGE("Erroneous response from MCU.");
		return E_ERROR;
	}

	return E_NONE;
}

/** @} */
