/**
@mainpage

<h3>Kuji16 Flash MCU Programmer</h3>
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

<h3>Introduction</h3>

The programming cycle is in two stages.
Stage 1 (Built-In-Rom on MCU) can accept executable code (the stage 2 binary) and store it in RAM. It can then execute (jump) to RAM.
Stage 2 (KERNAL) is then running from RAM and accepts the actual user application and writes it to FLASH in 512 byte blocks.

Stage 1:
	- Implemented by @link birom16 @endlink
	- Interfaces to ROM code on MCU that can only write to RAM and jump to that code.

Stage 2:
	- Implemented by @link kernal16 @endlink
	- Interface to flash loader that blank checks, erases, reads from and writes to flash.

Both stage 1 and 2 use CRC16.
	- Stage 1 only has a very simple checksum that sums up byte values of the data and negates the result (ones complement).
	- Stage 2 has a sort of standard ITT CRC16. See @link crcitt @endlink

<h3>Command line arguments</h3>
<pre>
-h           Print help and exit.
-H           Print all supported MCUs and exit.
-V           Print application version and exit.
-d           Hex-dump communication to stdout.
-v           Select verbosity level: 0-none, 1-errors, 2-warnings, 3-info, 4-debug.
-m \<mcu\>     Select MCU by name e.g. 'mb90f598g'. Case-insensitive.
-c \<freq\>    Select target crystal (megahertz) e.g 4, 8, 16 etc.
-e           Erase MCU flash.
-b           Blank check and return immediately after.
-r           Read MCU flash and write it to stdout as S-Records.
-w \<file\>    Write S-Record file to MCU flash.
-p \<com\>     Set com port Id from 1-99.
-p \<com\>     Set com port device e.g. '/dev/ttyS0'.
</pre>

<h3>Example usage</h3>

Blank-check MCU:
<pre>$./kuji16 -p1 -mmb90f598g -b</pre>

Take backup of MCU:
<pre>$./kuji16 -p1 -mmb90f598g -r > backupfirmware.mhx</pre>

Erase MCU:
<pre>$./kuji16 -p1 -mmb90f598g -e</pre>

Write S-Records to MCU:
<pre>$./kuji16 -p1 -mmb90f598g -w firmware.mhx</pre>

Do it all in one step:
<pre>$./kuji16 -p1 -mmb90f598g -r -e -w firmware.mhx > backupfirmware.mhx</pre>

<h3>Exit codes:</h3>
<ul>
<li>1 : Timed out waiting for processor. Check cable and stuff.
<li>2 : Was asked to blank check and the MCU was NOT blank.
<li>3 : Error reading from MCU.
<li>4 : Error writing to MCU.
<li>5 : Error in S-Record either file I/O or its data.
<li>6 : Error in communication detected by CRC.
<li>7 : Error in serial port such as access restrictions or errors in reading or writing.
<li>8 : Error in 'chipdef16.ini' either reading from it or in its data.
<li>9 : Error in one of the arguments either missing or invalid.
<li>10 : Error initializing Birom16 interface.
<li>11 : Error initializing Kernal interface.
<li>12 : Error blank-checking MCU.
<li>13 : Error erasing MCU.
</ul>

<h3>Supported MCUs:</h3>
<ul>
<li>MB90F334
<li>MB90F335A
<li>MB90F337
<li>MB90F342A/CA/AS/CAS
<li>MB90F342E/CE/ES/CES
<li>MB90F345A/CA/AS/CAS
<li>MB90F345E/CE/ES/CES
<li>MB90F346A/CA/AS/CAS
<li>MB90F346E/CE/ES/CES
<li>MB90F347A/CA/AS/CAS
<li>MB90F347E/CE/ES/CES
<li>MB90F349A/CA/AS/CAS
<li>MB90F349E/CE/ES/CES
<li>MB90F351/S
<li>MB90F351B/BS
<li>MB90F351A/AS/TA/TAS
<li>MB90F351E/ES/TE/TES
<li>MB90F352/S
<li>MB90F352B/BS
<li>MB90F352A/AS/TA/TAS
<li>MB90F352E/ES/TE/TES
<li>MB90F357A/AS/TA/TAS
<li>MB90F357E/ES/TE/TES
<li>MB90F362E/ES/TE/TES
<li>MB90F367E/ES/TE/TES
<li>MB90F372
<li>MB90F378
<li>MB90F387/S
<li>MB90F394H
<li>MB90F395H
<li>MB90MF408
<li>MB90F423GA/GC
<li>MB90F428GA/GC
<li>MB90F438L/LS
<li>MB90F439/S
<li>MB90F443G
<li>MB90F455/S
<li>MB90F456/S
<li>MB90F457/S
<li>MB90F462
<li>MB90F474H/L
<li>MB90F476/A
<li>MB90F481
<li>MB90F482
<li>MB90F488
<li>MB90F489
<li>MB90F497/G
<li>MB90F498G
<li>MB90F523B
<li>MB90F543/G/GS
<li>MB90F546G/GS
<li>MB90F548G/GS
<li>MB90F549/G/GS
<li>MB90F553A
<li>MB90F562/B
<li>MB90F568
<li>MB90F574/A
<li>MB90F583B/C/CA
<li>MB90F584C/CA
<li>MB90F591A/G
<li>MB90F594A/G
<li>MB90F598/G
<li>MB90F654A
<li>MB90F803
<li>MB90F804
<li>MB90F822
<li>MB90F823
<li>MB90F828B
<li>MB90F867A/AS
<li>MB90F867E/ES
<li>MB90F882
<li>MB90F883/A/B
<li>MB90F883C
<li>MB90F884/A/B/C
<li>MB90F897/S
<li>MB90F912
<li>MB90F922
<li>MB90F923
<li>MB90F924
<li>MB90F946A
<li>MB90F947A
<li>MB90F949A
<li>MB90F952
<li>MB90F962
<li>MB90F983
<li>MB90F997
</ul>

@author Kari Sigurjonsson
@defgroup prog16 16 Bit Programmer
@{
*/
#ifndef __PROG16_H__
#define __PROG16_H__

/**
	Enumeration of 16 bit Fujitsu MCU types.
*/
enum mcu16_type {
	MCU16_INVALID = 0x00,	/**< Not a valid MCU Id. Do not remove. */
	MCU16_MB90F334,
	MCU16_MB90F335A,
	MCU16_MB90F337,
	MCU16_MB90F342A,
	MCU16_MB90F342E,
	MCU16_MB90F345A,
	MCU16_MB90F345E,
	MCU16_MB90F346A,
	MCU16_MB90F346E,
	MCU16_MB90F347A,
	MCU16_MB90F347E,
	MCU16_MB90F349A,
	MCU16_MB90F349E,
	MCU16_MB90F351S,
	MCU16_MB90F351B,
	MCU16_MB90F351A,
	MCU16_MB90F351E,
	MCU16_MB90F352S,
	MCU16_MB90F352B,
	MCU16_MB90F352A,
	MCU16_MB90F352E,
	MCU16_MB90F357A,
	MCU16_MB90F357E,
	MCU16_MB90F362E,
	MCU16_MB90F367E,
	MCU16_MB90F372,
	MCU16_MB90F378,
	MCU16_MB90F387S,
	MCU16_MB90F394H,
	MCU16_MB90F395H,
	MCU16_MB90MF408,
	MCU16_MB90F423G,
	MCU16_MB90F428G,
	MCU16_MB90F438L,
	MCU16_MB90F439S,
	MCU16_MB90F443G,
	MCU16_MB90F455S,
	MCU16_MB90F456S,
	MCU16_MB90F457S,
	MCU16_MB90F462,
	MCU16_MB90F474H,
	MCU16_MB90F476A,
	MCU16_MB90F481,
	MCU16_MB90F482,
	MCU16_MB90F488,
	MCU16_MB90F489,
	MCU16_MB90F497G,
	MCU16_MB90F498G,
	MCU16_MB90F523B,
	MCU16_MB90F543G,
	MCU16_MB90F546G,
	MCU16_MB90F548G,
	MCU16_MB90F549G,
	MCU16_MB90F553A,
	MCU16_MB90F562B,
	MCU16_MB90F568,
	MCU16_MB90F574A,
	MCU16_MB90F583B,
	MCU16_MB90F584C,
	MCU16_MB90F591A,
	MCU16_MB90F594A,
	MCU16_MB90F598G,
	MCU16_MB90F654A,
	MCU16_MB90F803,
	MCU16_MB90F804,
	MCU16_MB90F822,
	MCU16_MB90F823,
	MCU16_MB90F828B,
	MCU16_MB90F867A,
	MCU16_MB90F867E,
	MCU16_MB90F882,
	MCU16_MB90F883A,
	MCU16_MB90F883C,
	MCU16_MB90F884A,
	MCU16_MB90F897S,
	MCU16_MB90F912,
	MCU16_MB90F922,
	MCU16_MB90F923,
	MCU16_MB90F924,
	MCU16_MB90F946A,
	MCU16_MB90F947A,
	MCU16_MB90F949A,
	MCU16_MB90F952,
	MCU16_MB90F962,
	MCU16_MB90F983,
	MCU16_MB90F997,
	//--
	MAX_MCU16_TYPE		/**< Sentinel. */
};

/** MCU nametag. */
struct mcu16_tag {
	char *name;				/**< MCU name as single word. */
	enum mcu16_type type;	/**< MCU type Id. */
};

/** Array of MCU names and associated type identifiers. */
extern struct mcu16_tag mcu16_map[];

/** Enumeration of supported crystal frequencies. */
enum frequency {
	FREQ_3MHZ		= 3000000,	/**< 3 megahertz. */
	FREQ_4MHZ		= 4000000,	/**< 4 megahertz. */
	FREQ_5MHz		= 5000000,	/**< 5 megahertz. */
	FREQ_6MHz		= 8000000,	/**< 8 megahertz. */
	FREQ_8MHZ		= 8000000,	/**< 8 megahertz. */
	FREQ_10MHz		= 10000000,	/**< 10 megahertz. */
	FREQ_12MHz		= 12000000,	/**< 12 megahertz. */
	FREQ_16MHZ		= 16000000,	/**< 16 megahertz. */
	FREQ_20MHz		= 20000000, /**< 20 megahertz. */
	FREQ_24MHz		= 24000000,	/**< 24 megahertz. */
};

/** Total number of frequencies supported. This is to size arrays and such. */
#define N_FREQUENCY		10

/** List of all available crystal frequencies. */
int frequencies[N_FREQUENCY];

/**
	A list of valid bit rates.
*/
enum bps {
	BPS_INVALID	= 0,
	BPS_110		= 110,
	BPS_150		= 150,
	BPS_300		= 300,
	BPS_1200	= 1200,
	BPS_2400	= 2400,
	BPS_4800	= 4800,
	BPS_9600	= 9600,
	BPS_19200	= 19200,
	BPS_38400	= 38400,
	BPS_57600	= 57600,
	BPS_115200	= 115200,
	BPS_230400	= 230400,
	BPS_460800	= 460800,
	BPS_921600	= 921600,
};

/** Total number of bit rates supported. This is to size arrays and such. */
#define N_BPS	15

/** Configuration of a 16 bit MCU. */
struct chipdef16 {
	enum mcu16_type mcu;				/**< Tell what MCU this entry is for. */
	char name[256];						/**< Common name of MCU. */
	char kernal[256];					/**< Base name of the kernal file aka stage 2 boot loader. */
	enum frequency clock[N_FREQUENCY];	/**< Array of valid crystal frequencies when stage 1 is running. */
	enum bps bps[N_FREQUENCY];			/**< Array of valid baud rates for stage 1. This should complement clock[] item by item. */
	enum bps bps2[N_FREQUENCY];			/**< Array of valid baud rates for stage 2. This should complement clock[] item by item. */
	uint16_t address_load;				/**< Where in RAM to store stage 2 binary. This is usually 0x0990 or 0x0190. */
	uint32_t flash_start;				/**< Where in FLASH to store user firmware. 24 bit range. */
	uint32_t flash_end;					/**< Last address of flash. 24 bit range. */
	uint32_t flash_size;				/**< Flash size (flash_end - flash_start). 24 bit range. */
};

/**
	Holds configuration for each 16 bit MCU type.
	This array is initialized with data from 'chipdef16.ini'.
*/
extern struct chipdef16 chipdefs[MAX_MCU16_TYPE];

/**
This mambo processes 'chipdef16.ini' file that describes each supported processor.
The global array chipdefs[] receives the processed data.
@return On success, returns E_NONE.
@return On failure, returns a negative error code.
*/
int process_chipdef16();

/**
	Find MCU type by name.
	@param s MCU name.
	@return On success, returns the MCU identifier.
	@return On failure i.e. no such MCU, returns a negative error code.
*/
int find_mcu16_by_name(char *s);

/**
	Look up common MCU name given a type identifier.
	@param type MCU type identifier.
	@return On success, returns the name.
	@return On failure i.e. invalid type, returns NULL.
*/
const char *mcu16_name(enum mcu16_type type);

#endif //__PROG16_H__

/** @} */
