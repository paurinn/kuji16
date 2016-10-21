/*
Kuji16 Flash MCU Programmer
Copyright (C) 2014-2016 Kari Sigurjonsson

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

int birom16_new(struct birom16_state **state, struct chipdef16 *chip, struct serial *serial) {
	char path[256];
	int rc;

	assert(state);

	*state = calloc(1, sizeof(struct birom16_state));
	assert(*state);

	(*state)->chip = chip;
	(*state)->serial = serial;

	LOGD("User says MCU is '%s' on '%s'.", chip->name, serial->address);

	memset(path, 0x00, sizeof(path));
	snprintf(path, sizeof(path) - 1, "kernal16/%s", chip->kernal);

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
		free(*state);
		*state = NULL;
	}
}

int birom16_connect(struct birom16_state *state, uint8_t timeoutsec) {
	int rc;
	uint8_t buf[2];
	int tally = 0;

	serial_purge(state->serial);

	memset(&buf, 0x00, sizeof(buf));

	double timeout = get_ticks() + timeoutsec;
	while (get_ticks() < timeout) {
		msleep(100);

		buf[0] = BIROM16_CMD_PROBE;
		rc = serial_write(state->serial, buf, 1);
		if (rc < 1) {
			LOGE("Error writing to serial port! Aborting.");
			return rc;
		}

		serial_drain(state->serial);

		msleep(100);

		if ((tally % 10) == 0) {
			LOGI("Probing for BIROM %.0f...", ceil(timeout - get_ticks()));
		}
		++tally;

		memset(&buf, 0x00, sizeof(buf));
		while ((rc = serial_read(state->serial, buf, 1)) > 0) {
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

	LOGE("ERROR - Time-out waiting for MCU.");
	return E_TIMEOUT;
}

int birom16_write(struct birom16_state *state, uint16_t address, uint8_t *data, uint16_t size) {
	uint8_t buffer[4096];
	memset(buffer, 0x00, sizeof(buffer));

	serial_purge(state->serial);

	buffer[0] = BIROM16_CMD_WRITE;
	buffer[1] = (address & 0xFF00) >> 8;
	buffer[2] = (address & 0x00FF);
	buffer[3] = (size & 0xFF00) >> 8;
	buffer[4] = (size & 0x00FF);

	memcpy(buffer + 5, data, size);
	size += 5;

	uint8_t csum = checksum8(buffer, size);
	buffer[size++] = csum;

	LOGD("Writing %d bytes to address 0x%04X...", size, address);

	int rc = serial_write(state->serial, buffer, size);
	if (rc < size) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	//Give MCU some breathing space.
	serial_drain(state->serial);

	//Expect single byte result of operation from MCU.
	uint8_t code = 0;
	uint8_t value = 0;
	double timeout = get_ticks() + 2;
	while (get_ticks() < timeout) {
		msleep(10);
		rc = serial_read(state->serial, &code, 1);
		if (rc < 0) {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		}

		if (rc > 0) {
			if (code == BIROM16_RESP_ACK) {
				return E_NONE;
			}
		}
	}

	LOGE("Invalid MCU response: Code %d, value %d.", code, value);
	return E_MSGMALFORMED;
	//return E_NONE;
}

int birom16_call(struct birom16_state *state, uint16_t address) {
	uint8_t buffer[5];

	memset(buffer, 0x00, sizeof(buffer));

	buffer[0] = BIROM16_CMD_CALL;
	buffer[1] = (address & 0xFF000000) >> 24;
	buffer[2] = (address & 0x00FF0000) >> 16;
	buffer[3] = (address & 0x0000FF00) >> 8;
	buffer[4] = (address & 0x000000FF);

	serial_purge(state->serial);

	LOGD("Calling stage 2 at address 0x%04X.", address);

	unsigned int rc = serial_write(state->serial, buffer, sizeof(buffer));
	if (rc < sizeof(buffer)) {
		LOGE("Error writing to MCU.");
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Expect single byte result of operation from MCU.
	rc = serial_read(state->serial, buffer, sizeof(buffer) - 1);
	if (buffer[0] != 0x31) {
		LOGE("Erroneous response from MCU.");
		return E_ERROR;
	}

	return E_NONE;
}

/** @} */
