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
@addtogroup kernal16
@{
*/
#include "stdafx.h"

int kernal16_new(struct kernal16 **state, struct chipdef16 *chip, struct serial *serial) {
	int rc;

	*state = (struct kernal16 *)calloc(1, sizeof(struct kernal16));
	assert(*state);

	(*state)->chip = chip;
	(*state)->serial = serial;

	LOGD("User says MCU is '%s' on %s.", chip->name, serial->address);

	return E_NONE;
}

void kernal16_free(struct kernal16 **state) {
	if (state && *state) {
		free(*state);
		*state = NULL;
	}
}

int kernal16_intro(struct kernal16 *state) {
	uint8_t buf[10];
	int rc;
	int tally = 0;

	memset(buf, 0x00, sizeof(buf));

	serial_purge(state->serial);

	double timeout = get_ticks() + 2;
	while (get_ticks() < timeout) {
		msleep(10);

		buf[0] = KERNAL16_CMD_INTRO;
		rc = serial_write(state->serial, buf, 1);
		if (rc < 1) {
			LOGE("Error writing to serial port! Aborting.");
			return rc;
		}

		serial_drain(state->serial);

		msleep(100);

		if ((tally % 10) == 0) {
			LOGI("Probing for KERNAL %.0f...", ceil(timeout - get_ticks()));
		}
		++tally;

		memset(&buf, 0x00, sizeof(buf));
		while ((rc = serial_read(state->serial, buf, 1)) > 0) {
			if (buf[0] == KERNAL16_RESP_ACK) {
				return E_NONE;	//Returning zero to mean MCU is alive.
			}
		}

		if (rc < 0) {
			LOGE("Error reading from serial port! Aborting.");
			return rc;
		}

		if (buf[0] > 0 && buf[0] != KERNAL16_RESP_ACK) {
			LOGE("Malformed response from MCU (0x%X). Please power off board and try again.", buf[0]);
			return E_MSGMALFORMED;
		}
	}

	LOGE("Time-out waiting for MCU.");
	return E_TIMEOUT;
}

int kernal16_blankcheck(struct kernal16 *state, uint32_t flash_base) {
	uint8_t buf[10];
	int rc;

	memset(buf, 0x00, sizeof(buf));

	serial_purge(state->serial);

	buf[0] = KERNAL16_CMD_BLANKCHECK;
	buf[3] = (flash_base & 0xFF0000) >> 16;
	buf[2] = (flash_base & 0x00FF00) >> 8;
	buf[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, buf, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	rc = serial_read(state->serial, buf, 1);

	int retry = 100;
	while (rc >= 0 && retry--) {
		if (rc < 0) {
			LOGE("Read error.");
			return E_READ;
		} else if (rc > 0) {
			if (buf[0] == KERNAL16_RESP_BUSY) {
				//LOGW("...MCU busy...");
			} else if (buf[0] == KERNAL16_RESP_ACK) {
				return 1;
			} else if (buf[0] == KERNAL16_RESP_BLANK) {
				//Read 4 bytes (address) and 2 bytes (data).
				rc = serial_read(state->serial, buf, 6);
				if (rc < 6) {
					LOGE("Error reading address and data from 0x34 response.");
					return E_MSGMALFORMED;
				}
				uint32_t address = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
				uint16_t data = buf[4] << 8 | buf[5];
				LOGD("Chip is NOT blank. Address 0x%04X has value 0x%02X.", address, data);

				return 0;
			} else {
				LOGE("Erroneounus data received %02X", buf[0]);
			}
		} else {
			msleep(1);
		}
		rc = serial_read(state->serial, buf, 1);
	}

	LOGE("ERROR: Time-out waiting for MCU.");

	return E_TIMEOUT;
};

int kernal16_erasechip(struct kernal16 *state, uint32_t flash_base) {
	uint8_t buf[256];
	int rc;

	memset(buf, 0x00, sizeof(buf));

	serial_purge(state->serial);

	buf[0] = KERNAL16_CMD_ERASECHIP;
	buf[3] = (flash_base & 0xFF0000) >> 16;
	buf[2] = (flash_base & 0x00FF00) >> 8;
	buf[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, buf, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive busy marker.
	rc = serial_read(state->serial, buf, 1);

	//Receive ACK or NAK.

	int retry = 60;
#ifdef __WIN32__
	int count = 1;
#endif
	while (retry--) {
		rc = serial_read(state->serial, buf, 1);
		if (rc < 0) {
			LOGE("Read error.");
			return E_READ;
		} else if (rc > 0) {
			if (buf[0] == KERNAL16_RESP_BUSY) {
				//LOGW("...MCU busy...");
				retry = 1000;
			} else if (buf[0] == KERNAL16_RESP_ACK) {
				return E_NONE;	//Return success.
			} else if (buf[1] == KERNAL16_RESP_NAK) {
				LOGE("ERROR - Chip NOT erased.");
				return E_FULL;
			} else {
				LOGE("Erroneous reply from MCU: ");
				return E_MSGMALFORMED;
			}
		} else {
#ifdef __WIN32__
			//LOGR("Erasing %d", count++);
			LOGR("!! DO NOT UNPLUG POWER !!\nErasing %d", count++);
#else
			LOGR("#");
#endif
			msleep(250);
		}
	}

	LOGE("ERROR: Time-out waiting for MCU.");

	return E_TIMEOUT;
}

int kernal16_readflash(struct kernal16 *state, uint32_t flash_base, uint8_t *buf, uint16_t size, uint16_t *pcsum) {
	int rc;

	memset(buf, 0xFF, size);

	serial_purge(state->serial);

	uint8_t cmd[4];

	cmd[0] = KERNAL16_CMD_READFLASH;
	cmd[3] = (flash_base & 0xFF0000) >> 16;
	cmd[2] = (flash_base & 0x00FF00) >> 8;
	cmd[1] = (flash_base & 0x0000FF);

	rc = serial_write(state->serial, cmd, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive 'busy' marker...
	rc = serial_read(state->serial, buf, 1);
	if (rc < 1 || buf[0] != KERNAL16_RESP_BUSY) {
		LOGE("ERROR: Did not receive busy marker after READ command.");
		return rc < 0 ? E_READ : E_MSGMALFORMED;
	}

	//...and ACK marker.
	rc = serial_read(state->serial, buf, 1);
	if (rc < 1 || buf[0] != KERNAL16_RESP_ACK) {
		LOGE("ERROR: Did not receive acknowledge to READ command.");
		return rc < 0 ? E_READ : E_MSGMALFORMED;
	}

	int i = 0;
	int retry = 30;
	while (i < size) {
		rc = serial_read(state->serial, buf + i, size - i);
		if (rc < 0) {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		} else if (rc > 0) {
			i += rc;
			retry = 30;
		} else if (rc == 0) {
			msleep(1);
			if (retry-- > 0 && i < size) continue;
		}
	};

	//Receive checksum and final confirmation.
	uint8_t csumok[3];
	i += serial_read(state->serial, csumok, 3);

	//CRC value from MCU.
	uint16_t pkcrc = ((csumok[0] << 8) & 0xFF00) | csumok[1];

	//CRC value as computed by us.
	uint16_t mycrc = crcitt(buf, size);

	if (pcsum) *pcsum = pkcrc;

	if (i < size + 3 || csumok[2] != KERNAL16_RESP_ACK) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	if (mycrc != pkcrc) {
		LOGE("ERROR: CRC mismatch. Packet CRC 0x%04X is not 0x%04X.", pkcrc, mycrc);
		return E_MSGMALFORMED;
	}

	return E_NONE;
}

int kernal16_writeflash(struct kernal16 *state, uint32_t flash_base, uint8_t *buf, uint16_t size) {
	int rc;

	serial_purge(state->serial);

	uint8_t cmd[6];

	cmd[0] = KERNAL16_CMD_WRITEFLASH;
	cmd[1] = (flash_base & 0xFF);
	cmd[2] = (flash_base >> 8) & 0xFF;
	cmd[3] = (flash_base >> 16) & 0xFF;

	rc = serial_write(state->serial, cmd, 4);
	if (rc < 4) {
		LOGE("Could not write to: %s.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	//Receive 'busy' marker.
	rc = serial_read(state->serial, cmd, 1);
	if (rc < 1) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	//Receive 'ready' marker.
	rc = serial_read(state->serial, cmd, 1);
	if (rc < 1) {
		LOGE("Error reading from '%s'.", state->serial->address);
		return E_READ;
	}

	rc = serial_write(state->serial, buf, size);
	if (rc < size) {
		LOGE("Error writing to '%s'.", state->serial->address);
		return E_WRITE;
	}

	serial_drain(state->serial);

	uint16_t crc = crcitt(buf, size);
	cmd[0] = crc >> 8;
	cmd[1] = crc;
	rc = serial_write(state->serial, cmd, 2);
	if (rc < 1) {
		LOGE("ERROR: Error writing checksum to '%s'.", state->serial->address);
		return E_WRITE;
	}

	// Wait a bit for the data to go down the wire and then read busy marker and confirmation.
	serial_drain(state->serial);

	memset(&cmd, 0x00, sizeof(cmd));
	double timeout = get_ticks() + 1;
	while (get_ticks() < timeout) {
		msleep(10);
		rc = serial_read(state->serial, cmd, 1);
		if (rc < 0) {
			LOGE("Error reading from '%s'.", state->serial->address);
			return E_READ;
		}

		if (rc > 0) {
			if (cmd[0] == KERNAL16_RESP_ERRCRC) {
				LOGE("CRC error in communication.");
				return E_CRC;
			} else if (cmd[0] == KERNAL16_RESP_ACK) {
				return E_NONE;
			}
		}
	}

	LOGE("Timed out waiting for MCU.");
	return E_TIMEOUT;
}

/** @} */

