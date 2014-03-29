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
@addtogroup prog16
@{
*/
#include "stdafx.h"

/** Enumeration of failure codes this program returns. */
enum failures {
	/**
		Timed out waiting for processor.
		This usually means the MCU has no power, program jumper not in place or
		connection failure in RS232 cable.
	*/
	EXIT_FAIL_TIMEOUT		= 1,
	EXIT_FAIL_NOTBLANK		= 2,	/**< Was asked to blank check or write and the MCU was NOT blank. */
	EXIT_FAIL_ISBLANK		= 3,	/**< Was asked to blank check or read and the MCU was blank. */
	EXIT_FAIL_READ			= 4,	/**< Error reading from MCU. */
	EXIT_FAIL_WRITE			= 5,	/**< Error writing to MCU. */
	EXIT_FAIL_SRECORD		= 6,	/**< Error in S-Record either file I/O or its data. */
	EXIT_FAIL_CRC			= 7,	/**< Error in communication detected by CRC. */
	EXIT_FAIL_SERIAL		= 8,	/**< Error in serial port such as access restrictions or errors in reading or writing. */
	EXIT_FAIL_CHIPDEF		= 9,	/**< Error in 'chipdef16.ini' either reading from it or in its data. */
	EXIT_FAIL_ARGUMENT		= 10,	/**< Error in one of the arguments either missing or invalid. */
	EXIT_FAIL_INITBIROM		= 11,	/**< Error initializing Birom16 interface. */
	EXIT_FAIL_INITKERNAL	= 12,	/**< Error initializing Kernal interface. */
	EXIT_FAIL_BLANK			= 13,	/**< Error blank-checking MCU. */
	EXIT_FAIL_ERASE			= 14,	/**< Error erasing MCU. */
};

/** License clause. */
const char *license = "\
\n\
Copyright (C) 2014  Kari Sigurjonsson\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";

/** Help clause. */
const char *help = "\
\n\
--------------------------------\n\
Usage: ./kuji16 -m <mcu> -p <com> [-c <freq>] [-r] [-e] [-v] [-d] [-q] [-w <somefile.mhx>]\n\
  -h         Print help and exit.\n\
  -H         Print all supported MCUs and exit.\n\
  -V         Print application version and exit.\n\
  -d         Hex-dump communication to stdout.\n\
  -v         Select verbosity level: 0-none, 1-errors, 2-warnings, 3-info, 4-debug.\n\
  -l <file>  Write log to <file> instead of main.log.\n\
  -p <com>   Set com port Id from 1-99 on Windows or com port device e.g. '/dev/ttyS0' on Linux.\n\
  -m <mcu>   Select MCU by name e.g. 'mb90f598g'. Case-insensitive.\n\
  -c <freq>  Select target crystal (megahertz) e.g 4, 8, 16 etc. Default is 4 Mhz.\n\
  -b         Blank-check and exit immediately after.\n\
  -r <file>  Read MCU flash and write it as S-Records to file.\n\
  -e         Erase MCU flash.\n\
  -w <file>  Write S-Record file to MCU flash.\n\
\n\
Example: ./kuji16 -m mb90f598g -p1 -e -w firmware.mhx\n\
\n\
To print out supported MCUs: ./kuji16 -H\n\
\n\
Note: The file must be standard Motorola S-Record.\n\
\n\
Exit codes:\n\
--------------\n\
1 : Timed out waiting for processor. Check cable and stuff.\n\
2 : Was asked to blank check or write and the MCU was NOT blank.\n\
3 : Was asked to blank check or read and the MCU was blank.\n\
4 : Error reading from MCU.\n\
5 : Error writing to MCU.\n\
6 : Error in S-Record either file I/O or its data.\n\
7 : Error in communication detected by CRC.\n\
8 : Error in serial port such as access restrictions or errors in reading or writing.\n\
9 : Error in 'chipdef16.ini' either reading from it or in its data.\n\
10 : Error in one of the arguments either missing or invalid.\n\
11 : Error initializing Birom16 interface.\n\
12 : Error initializing Kernal interface.\n\
13 : Error blank-checking MCU.\n\
14 : Error erasing MCU.\n\
";
/**
Print out usage information to stdout.
*/
void print_help() {
	LOGR("%s%s%s",
		version_string(),
		help,
		license
	);
}

/**
	Test if all bytes in a given buffer are 0xFF.
	@param buf Buffer of bytes to test.
	@param size Size of buf[].
	@return If buffer only contains 0xFF, returns true.
*/
bool isflashbufempty(uint8_t *buf, int size);

inline bool isflashbufempty(uint8_t *buf, int size) {
	while (size--) {
		if (*buf != 0xFF) return false;
		buf++;
	}
	return true;
}


/**
	Main entry point.
	Process chipdef file, parse user options and run the programming state machine.
*/
int main(int argc, char *argv[]) {
	int bps = 0;
	int id = 0;
	int rc;
	int opt;
	int freqid = -1;
	enum frequency freq = 0;
	struct chipdef16 *chip = NULL;
	bool debugging = false;
	int bytes = 0;
	uint16_t csum = 0;

	char *argstr = "hHVdl:v:p:m:c:ber:w:";
	char *srecpath = NULL;
	char *savepath = NULL;

#ifdef __WIN32__
	char path[256];
	int comid = 0;
#else
	char *compath = NULL;
#endif

	if (process_chipdef() != E_NONE) {
		return EXIT_FAIL_CHIPDEF;
	}

	bool erase = false;
	bool read = false;
	bool write = false;
	bool isblank = false;
	bool blankcheck = false;

	while ((opt = getopt(argc, argv, argstr)) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				return EXIT_SUCCESS;

			case 'H':
				LOGI("\nSupported MCUs:");
				for (id = 1; mcu16_map[id].name; id++) {
					LOGI("%s", mcu16_map[id].name);
				}
				LOGI("--------------------------");
				return EXIT_SUCCESS;

			case 'V':
				LOGR("%s\n%s\n", version_string(), license);
				return EXIT_SUCCESS;

			case 'd':
				debugging = true;
				break;

			case 'v':
				verbosity = strtoint32(optarg, 10, NULL);
				LOGD("Verbosity: %d", verbosity);
				break;

			case 'l':
				if (optarg) {
					loggpath = optarg;
				} else if (argv[optind] && argv[optind][0] && argv[optind][0] != '-') {
					loggpath = argv[optind];
					optind++;
				}
				LOGD("Using '%s' for log.", loggpath);
				break;

			case 'p':
#ifdef __WIN32__
				id = strtoint32(optarg, 10, NULL);
				if (id > 0 && id < 100) {
					comid = id;
				}
#else
				compath = optarg;
#endif
				break;

			case 'm':
				id = find_mcu16_by_name(optarg);
				if (id > 0) {
					chip = &chipdefs[id];
				} else {
					LOGE("ERROR: Invalid option '%s' to -m. This is not a MCU name.", optarg);
					return EXIT_FAIL_ARGUMENT;
				}
				break;

			case 'c':
				id = strtoint32(optarg, 10, NULL);
				id *= 1000000;
				for (rc = 1; rc < N_FREQUENCY; rc++) {
					if (frequencies[rc] == id) {
						freq = frequencies[rc];
					}
				}
				break;

			case 'b':
				blankcheck = true;
				break;

			case 'r':
				savepath = optarg;
				if (savepath && savepath[0] == '\0') {
					savepath = NULL;
				}
				read = true;
				break;

			case 'e':
				erase = true;
				break;

			case 'w':
				srecpath = optarg;
				if (srecpath && srecpath[0] == '\0') {
					srecpath = NULL;
				}
				write = true;
				break;

			case '?':
				LOGE("Argument error!");
				return EXIT_FAIL_ARGUMENT;
		}
	}

	if (chip == NULL) {
		LOGE("ERROR: Missing or invalid option '-m'.\n");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

#ifdef __WIN32__
	if (comid <= 0) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}
#else
	if (compath == NULL) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}
#endif

	if (write && srecpath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-w'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	if (read && savepath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-r'.");
		print_help();
		return EXIT_FAIL_ARGUMENT;
	}

	//Morph frequency into MCU specific index that we select bps by.
	//This is because serial port bitrate follows clock selection.
	if (freq <= 0) {
		freq = FREQ_4MHZ;
		LOGD("Using default frequency %dHz.", freq);
	}

	for (id = 0; id < N_FREQUENCY; id++) {
		if (chip->clock[id] == freq) {
			freqid = id;
			break;
		}
	}

	if (freqid < 0) {
		LOGE("The clock frequency '%d' is not supported on MCU '%s'.", freq, mcu16_name(chip->mcu));
		return EXIT_FAIL_ARGUMENT;
	}

	//NOTE, bps follows the clock.
	bps = chip->bps[freqid];

	if (bps <= 0) {
		LOGE("Internal error: Invalid bitrate.");
		return EXIT_FAIL_ARGUMENT;
	}

	/****************************************************************************
	  Stage 1 BIROM (Built-In-ROM).
	 ****************************************************************************/
	struct birom16_state *birom = NULL;

	LOGD("---------- BIROM16 START ----------");

#ifdef __WIN32__
	snprintf(path, sizeof(path), "\\\\.\\COM%d", comid);
	rc = birom16_new(&birom, chip, freqid, path);
#else
	rc = birom16_new(&birom, chip, freqid, compath);
#endif
	if (rc != E_NONE) {
		birom16_free(&birom);
		return EXIT_FAIL_INITBIROM;
	}

	birom->serial.debug = debugging;

	//First handshake is to give user chance to power up MCU.
	LOGI("Probing for MCU. Please apply power to board...");

	//Is the audience listening?
	rc = birom16_connect(birom);
	if (rc != E_NONE) {
		birom16_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	//Dump stage 2 binary into MCU RAM.
	rc = birom16_write(birom, chip->address_load, birom->kernaldata, birom->kernalsize);
	if (rc != E_NONE) {
		birom16_free(&birom);
		return EXIT_FAIL_WRITE;
	}

	//Check if birom is still there.
	rc = birom16_connect(birom);
	if (rc != E_NONE) {
		birom16_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	// Call stage 1 and transfer control to stage 2 kernal16.
	rc = birom16_call(birom, chip->address_load);
	if (rc != E_NONE) {
		birom16_free(&birom);
		return EXIT_FAIL_TIMEOUT;
	}

	birom16_free(&birom);

	LOGD("---------- BIROM16 DONE ----------\n");

	/****************************************************************************
	  Stage 2 KERNAL
	 ****************************************************************************/
	struct kernal16 *kernal = NULL;

	LOGD("========== KERNAL16 START ==========");

#ifdef __WIN32__
	snprintf(path, sizeof(path), "\\\\.\\COM%d", comid);
	rc = kernal16_new(&kernal, chip, freqid, path);
#else
	rc = kernal16_new(&kernal, chip, freqid, compath);
#endif
	if (rc != E_NONE) {
		kernal16_free(&kernal);
		return EXIT_FAIL_INITKERNAL;
	}

	kernal->serial.debug = debugging;

	//Is the audience listening?
	rc = kernal16_intro(kernal);
	if (rc != E_NONE) {
		kernal16_free(&kernal);
		return EXIT_FAIL_TIMEOUT;
	}

	//Always blank-check.
	rc = kernal16_blankcheck(kernal, chip->flash_start);
	if (rc < 0) {
		LOGE("ERROR: Could not perform blank check!");
		kernal16_free(&kernal);
		return EXIT_FAIL_BLANK;
	}
	isblank = (rc == 1);

	//Exit early if chip is full and there are no operations or
	//if chip is empty and operations would not be possible.
	if (
		(blankcheck)
		|| (isblank && (!write))
		|| ((isblank && (read || erase) && !write))
		|| (read && isblank)
		|| (!isblank && (!write && !read && !erase))
	) {
		LOGI("== Chip Is %s ==", (isblank) ? "Blank" : "Not Blank");
		LOGD("========== KERNAL16 DONE ==========");
		kernal16_free(&kernal);
		return isblank ? EXIT_FAIL_ISBLANK: EXIT_FAIL_NOTBLANK;
	}

	//Read flash contents
	if (read) {
		if (isblank) {
			LOGI("== Chip Is Blank ==");
			LOGD("========== KERNAL16 DONE ==========");
			kernal16_free(&kernal);
			return EXIT_FAIL_ISBLANK;
		}

		//Sector by sector!
		uint8_t *buff = calloc(1, chip->flash_size * 2);
		assert(buff);

		bytes = 0;
		csum = 0;

		LOGR("[INF]: Reading ");
		for (uint32_t addr = chip->flash_start; addr < chip->flash_start + chip->flash_size; addr += 0x200, bytes += 512) {
			rc = kernal16_readflash(kernal, addr, buff + bytes, 512, &csum);
			if (rc != E_NONE) {
				LOGE("Error receiving flash contents.");
				kernal16_free(&kernal);
				return EXIT_FAIL_READ;
			}

#ifdef DEBUGGING
			LOGR("\rReceiving 512 bytes from sector 0x%06X, last CRC16 0x%04X", addr, csum);
#else
			if (bytes % 4096 == 0) {
				LOGR("#");
			}
#endif
		}

		LOGR("\n");

		rc = srec_writefilebin(buff, bytes, savepath, 2, chip->flash_start);
		if (rc != E_NONE) {
			LOGE("Error serializing S-Records.");
			kernal16_free(&kernal);
			return EXIT_FAIL_SRECORD;
		}

		free(buff);
		buff = NULL;

		LOGI("== Chip Read Successfully ==");
	}

	//Return early is chip is full and trying to write without erasing first.
	if (!isblank && write && !erase) {
		LOGE("Error: Trying to write into an already full MCU. Did you forget to add '-e' argument?");
		LOGD("========== KERNAL16 DONE ==========");
		kernal16_free(&kernal);
		return EXIT_FAIL_NOTBLANK;
	}

	//Erase chip.
	if (erase && !isblank) {
		LOGR("[INF]: Erasing ");
		rc = kernal16_erasechip(kernal, chip->flash_start);
		if (rc != E_NONE) {
			LOGR("\n");
			LOGE("ERROR: Could not erase flash!");
			kernal16_free(&kernal);
			return EXIT_FAIL_ERASE;
		}
		LOGR("\n");
		LOGI("== Chip Erased ==");
	}

	//Program S-Record.
	if (write) {
		uint8_t *buf = NULL;

		//Copy flash data from S-Records in file into a linear buffer.
		//The buffer is already 2^24 bytes so we can index it directly from chip->flash_start to chip->flash_end inclusively.
		rc = srec_readfilebin(&buf, srecpath, chip->flash_start, chip->flash_end);
		if (rc != E_NONE || buf == NULL) {
			LOGE("ERROR: Could not interpret S-Records from file '%s'.", srecpath);
			kernal16_free(&kernal);
			return EXIT_FAIL_SRECORD;
		}

		LOGD("Loaded S-Records from '%s'.", srecpath);

		bytes = 0;

		//Write out linear buffer into MCU flash in 512 byte chunks.
		LOGD("Writing 0x%06X bytes...", chip->flash_size);
		LOGR("[INF]: Writing ");
		for (uint32_t addr = chip->flash_start; addr < chip->flash_end; addr += 512, bytes += 512) {
			if (isflashbufempty(buf + addr, 512) == false) {
				rc = kernal16_writeflash(kernal, addr, buf + addr, 512);
				if (rc != E_NONE) {
					kernal16_free(&kernal);
					return EXIT_FAIL_WRITE;
				}

				if (bytes % 4096 == 0) {
					LOGR("#");
				}
			}
		}
		LOGR("\n");

		free(buf);
		buf = NULL;

		LOGI("== Chip Programmed Successfully ==");

	}

	kernal16_free(&kernal);

	LOGD("========== KERNAL16 DONE ==========");

	return EXIT_SUCCESS;
}
/** @} */

