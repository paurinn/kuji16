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
Interface to the temporary flash loader (stage 2) running in the target MCU RAM.
You must use @link birom16 @endlink to dump and run a stage 2 binary.

<h3>Commands to MCU</h3>
<table>
<tr><th>Command name in C</th><th>Code</th><th>What it does</th></tr>
<tr><td>KERNAL16_CMD_WRITEFLASH</td><td>0x08</td><td>Write data to flash. Size of packet 512 bytes + 2 bytes for crcitt() checksum.</td></tr>
<tr><td>KERNAL16_CMD_READFLASH</td><td>0x28</td><td>Read 512 byes of data from flash memory + 2 bytes of checksum for crcitt().</td></tr>
<tr><td>KERNAL16_CMD_ERASECHIP</td><td>0x38</td><td>Erase whole chip.</td></tr>
<tr><td>KERNAL16_CMD_BLANKCHECK</td><td>0x48</td><td>Test if block of memory is erased (filled with 0xFF).</td></tr>
<tr><td>KERNAL16_CMD_INTRO</td><td>0xE8</td><td>Only works as the first byte after birom16 call. Is this to initialize communication?</td></tr>
</table>

<h3>Responses from MCU.</h3>
<table>
<tr><th>Response name in C</th><th>Code</th><th>What</th></tr>
<tr><td>KERNAL16_RESP_BUSY</td><td>0x30</td><td>MCU is busy executing the current command, success/failure response follows after a while.</td></tr>
<tr><td>KERNAL16_RESP_ACK</td><td>0x31</td><td>Command was successful. No bytes follow.</td></tr>
<tr><td>KERNAL16_RESP_NAK</td><td>0x32</td><td>Command failed or communication error. No bytes follow.</td></tr>
<tr><td>KERNAL16_RESP_BLANK</td><td>0x34</td><td>Blank check failed. 32 bit address and 16 bit data follows.</td></tr>
<tr><td>KERNAL16_RESP_ERRCRC</td><td>0x35</td><td>CRC16 failed for block.</td></tr>
</table>

@defgroup kernal16 Stage 2 Interface to flash loader Kernal.
@{
*/
#ifndef __KERNAL16_H__
#define __KERNAL16_H__

/** Maximum number of in-memor S-Records. */
#define MAX_SRECORDS 0x800

/** Enumeration of all Kernal16 PC commands. */
enum kernal16_cmdid {
	KERNAL16_CMD_WRITEFLASH		= 0x08,	/**< Write data to flash. Size of packet 512 bytes. */
	KERNAL16_CMD_READFLASH		= 0x28, /**< Read 512 byes of data from flash memory. */
	KERNAL16_CMD_ERASECHIP		= 0x38,	/**< Erase whole chip. */
	KERNAL16_CMD_BLANKCHECK		= 0x48,	/**< Test if block of memory is erased (filled with 0xFF). */

	KERNAL16_CMD_INTRO			= 0xE8,	/**< Only works as the first byte after birom16 call. Is this to initialize communication? */
};

/** Enumeration of all Kernal16 MCU response codes. */
enum kernal16_resp {
	KERNAL16_RESP_BUSY		= 0x30,	/**< MCU is busy executing the current command, success/failure response follows after a while. */
	KERNAL16_RESP_ACK		= 0x31,	/**< Command was successful. No bytes follow. */
	KERNAL16_RESP_NAK		= 0x32,	/**< Command failed or communication error. No bytes follow. */
	KERNAL16_RESP_BLANK		= 0x34,	/**< Blank check failed. 32 bit address and 16 bit data follows. */
	KERNAL16_RESP_ERRCRC	= 0x35,	/**< CRC16 failed for block. */
};

/** Kernal16 State. */
struct kernal16 {
	struct chipdef16 *chip;		/**< Current chip configuration. */
	struct serial *serial;		/**< Serial communication. */
};

/**
Allocate for a new kernal16 state.
Opens up serial communication to com port.
Chip definitions tell MCU configuration.
@param state The dereferenced pointer will be assigned to the newly allocated kernal16 state.
@param chip Chip definition.
@param serial Serial port object.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int kernal16_new(struct kernal16 **state, struct chipdef16 *chip, struct serial *serial);

/**
Free or close all associated resources and then free *state.
@param state The dereferenced pointer is freed and assigned to NULL.
*/
void kernal16_free(struct kernal16 **state);

/**
Initialize communication to RAM Kernal.
Must be called only once before any other command.
@param state Kernal16 state.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int kernal16_intro(struct kernal16 *state);

/**
Check if the given memory range is blanko i.e. assigned 0xFF.
@param state Kernal16 state.
@param flash_base Starting address. 24 bit range.
@return On success if range is blank, returns E_NONE.
@return On failure i.e. if range is not blank or communication error, returns a negative error code.
*/
int kernal16_blankcheck(struct kernal16 *state, uint32_t flash_base);

/**
Erase whole chip.
@param state Kernal16 state.
@param flash_base Base address of flash.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int kernal16_erasechip(struct kernal16 *state, uint32_t flash_base);

/**
Write 512 bytes of data to flash.
This can take a while to finish.
@param state Kernal16 state.
@param flash_base Base address of flash sector.
@param buf Byte buffer to write.
@param size Number of bytes in buf[]. 512 bytes are always used, padding is 0xFF.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int kernal16_writeflash(struct kernal16 *state, uint32_t flash_base, uint8_t *buf, uint16_t size);

/**
Read 512 bytes of data from flash into buf.
@param state Kernal16 state.
@param flash_base Base address of flash sector.
@param buf Byte buffer to receive data.
@param size Number of bytes in buf[]. Must be at least 512 bytes.
@param pcsum Optional destination for checksum from message.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int kernal16_readflash(struct kernal16 *state, uint32_t flash_base, uint8_t *buf, uint16_t size, uint16_t *pcsum);

#endif //__KERNAL16_H__

/** @} */

