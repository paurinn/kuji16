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
Interface to Fujitsu 16LX MCU family built-in ROM bootstrap.
The bootstrap can only write to RAM so we implement a flash writer
and use the bootstrap to receive and call the executable.

Only three commands seem to be supported:
	- 0x18 - Initialize communication (doubles as PING).
	- 0x00 - Write at most 2^16 bytes to any address within MCU RAM.
	- 0x40 - Execute code. The address is fixed by built-in-ROM.

The MCU only sends 1 byte as response.
High nibble mirrors high nibble of command code.
Low nibble is result of operation (0x01=GOOD, 0x02=BAD).

This module relies on @link serial @endlink to abstract serial port access from architecture and OS but can be replaced with whatever easily.

@defgroup birom16 Stage 1 Interface to Built-In-ROM.
@{
*/
#ifndef __BIROM16_H__
#define __BIROM16_H__

/** Command identifiers. */
enum birom16_cmdid {
	BIROM16_CMD_PROBE	= 0x18,	/**< General communication check and communication initializer. MCU bootstrap responds with 0x11. */
	BIROM16_CMD_WRITE	= 0x00,	/**< Write data to RAM. MCU responds with 0x01 on success and 0x02 on failure. */
	BIROM16_CMD_CALL	= 0x40, /**< Jump to RAM address. */
};

/** Response identifiers. */
enum birom16_respid {
	BIROM16_RESP_INVALID	= 0x00, /**< Invalid response. Usually means time-out in communication. */
	BIROM16_RESP_ACK		= 0x1,	/**< Command was received and was successful. */
	BIROM16_RESP_NAK		= 0x2,	/**< Command was not fully received or command failed. */
	BIROM16_RESP_PROBE		= 0x11,	/**< Acknowledge to BIROM16_CMD_PROBE. */
};

/**
	Run-time state for birom16.
*/
struct birom16_state {
	struct chipdef16 *chip;		/**< Configuration of MCU such as crystal frequency, path to kernal file etc. */
	struct serial *serial;		/**< Serial communication object. */
	uint8_t *kernaldata;		/**< Contentes of kernal file. */
	long kernalsize;			/**< Size of kernal file. */
};

/**
	Allocate for a new birom16 state and initialize connection to the given serial port at the given baud rate.
	@param state The dereferenced pointer is assigned to the newly allocated state.
	@param chip Prepared chip definition describing the MCU being programmed.
	@param serial Serial port object.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int birom16_new(struct birom16_state **state, struct chipdef16 *chip, struct serial *serial);

/**
	Close serial port and free the given birom16 state.
	@param state The dereferenced pointer is freed and its value set to NULL.
*/
void birom16_free(struct birom16_state **state);

/**
	Connect to processor i.e. send BIROM16_CMD_OPEN and expect BIROM16_RESP_OK back.
	@param state The birom16 state.
	@param timeoutsec Time-out in seconds.
	@return On success, returns E_NONE.
	@return On failure, returns a negative error code.
*/
int birom16_connect(struct birom16_state *state, uint8_t timeoutsec);

/**
	Write size number of bytes to memory address in RAM.
	Ideal to dump flash loader executable.
	@param state The birom16 state.
	@param address Memory address.
	@param data Array of the bytes to write.
	@param size Number of bytes to write.
	@return On success, returns the number of bytes written.
	@return On failure, returns a negative error code.
*/
int birom16_write(struct birom16_state *state, uint16_t address, uint8_t *data, uint16_t size);

/**
	Make MCU jump to address. There is no return value from MCU as jump is immediate.
	@param state The birom16 state.
	@param address Dummy. The address is fixed by built-in-ROM.
*/
int birom16_call(struct birom16_state *state, uint16_t address);

#endif //__BIROM16_H__

/** @} */

