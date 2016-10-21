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
@addtogroup prog16
@{
*/
#include "stdafx.h"

/** Name of the chip definition file for 16 bit MCUs. */
const char *chipdef_filename = "chipdef16.ini";

/** List of all available crystal frequencies. */
int frequencies[N_FREQUENCY] = {
	FREQ_3MHZ,
	FREQ_4MHZ,
	FREQ_5MHz,
	FREQ_6MHz,
	FREQ_8MHZ,
	FREQ_10MHz,
	FREQ_12MHz,
	FREQ_16MHZ,
	FREQ_20MHz,
	FREQ_24MHz
};

/**
This is the default baudrate series in case chipdef16.ini does not provide a Baud2 series.
@note This probably needs changing.
*/
enum bps default_bps_series[N_FREQUENCY] = {
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
	BPS_9600,
};

/**
	Holds configuration for each 16 bit MCU type.
	This array is initialized with data from 'chipdef16.ini'.
*/
struct chipdef16 chipdefs[MAX_MCU16_TYPE];

int find_mcu16_by_name(char *s) {
	int rc = 0;
	for (int i = 0; i < MAX_MCU16_TYPE && chipdefs[i].name[0]; i++) {
		if ((rc = strcasecmp(chipdefs[i].name, s)) == 0) {
			return i;
		}
	}
	return E_NOTFOUND;
}

int process_chipdef16() {
	FILE *F = NULL;
	char *s;
	char line[256];
	char key[256];
	char value[256];
	int id = -1;
	int rc;

	memset(chipdefs, 0x00, sizeof(chipdefs));

	F = fopen(chipdef_filename, "r");
	if (F == NULL) {
		LOGE("Could not open file '%s' for reading.", chipdef_filename);
		return E_OPEN;
	}

	while ((s = fgets(line, sizeof(line) - 1, F))) {
		s = str_trim(s);
		if (s[0] == '\0' || s[0] == '\n') continue;
		if (s && s[0] == '[' && s[strlen(s) - 1] == ']') {
			s++;
			s[strlen(s) - 1] = '\0';

			++id;

			strncpy(chipdefs[id].name, s, sizeof(chipdefs[id].name) - 1);
			//LOGD("Processing [%s]", chipdefs[id].name);
		} else {
			if (id >= 0) {
				if (sscanf(s, "%[^=]=%s", key, value) < 2) {
					LOGE("Invalid chipdef line: '%s'.", s);
					continue;
				}

				if (strcasecmp(key, "DownloadFile") == 0) {
					//Base name of the kernal file aka stage 2 boot loader.
					strncpy(chipdefs[id].kernal, value, sizeof(chipdefs[id].kernal) - 1);
					//LOGI("\t%s.kernal = %s", chipdefs[id].name, chipdefs[id].kernal);
				} else if (strcasecmp(key, "LoadAddress") == 0) {
					//Where in RAM to store stage 2 binary. This is usually 0x0990 or 0x0190.
					chipdefs[id].address_load = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGI("\t%s.address_load = 0x%X", chipdefs[id].name, chipdefs[id].address_load);
				} else if (strcasecmp(key, "StartAddress") == 0) {
					//uint16_t flash_start;			/**< Where in FLASH to store user firmware. */
					chipdefs[id].flash_start = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGI("\t%s.flash_start = 0x%X", chipdefs[id].name, chipdefs[id].flash_start);
				} else if (strcasecmp(key, "EndAddress") == 0) {
					//uint16_t flash_end;				/**< Last address of flash. */
					chipdefs[id].flash_end = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGI("\t%s.flash_end = 0x%X", chipdefs[id].name, chipdefs[id].flash_end);
				} else if (strcasecmp(key, "FlashSize") == 0) {
					//uint16_t flash_size;			/**< Flash size (flash_end - flash_start). */
					chipdefs[id].flash_size = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGI("\t%s.flash_size = 0x%X", chipdefs[id].name, chipdefs[id].flash_size);
				} else if (strcasecmp(key, "Clock") == 0) {
					char *saveptr = value;
					char *seg;
					char *s = value;
					int c = 0;
					int cv = 0;

					for (c = 0; ; c++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						//NOTE, assuming value is megahertz
						sscanf(seg, "%d", &cv);
						cv *= 1000000;
						chipdefs[id].clock[c] = cv;
						//LOGI("\t%s.clock[%d] = %u", chipdefs[id].name, c, chipdefs[id].clock[c]);
					}
				} else if (strcasecmp(key, "Baud") == 0) {
					char *saveptr = value;
					char *seg;
					char *s = value;
					int b = 0;
					int br = 0;

					for (b = 0; ; b++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						//NOTE, assuming value is megahertz
						sscanf(seg, "%d", &br);
						chipdefs[id].bps[b] = br;
						//LOGI("\t%s.bps[%d] = %u", chipdefs[id].name, b, chipdefs[id].bps[b]);
					}
				} else if (strcasecmp(key, "Baud2") == 0) {
					char *saveptr = value;
					char *seg;
					char *s = value;
					int b = 0;
					int br = 0;

					for (b = 0; ; b++, s = NULL) {
						seg = strtok_r(s, ",", &saveptr);
						if (seg == NULL) break;
						//NOTE, assuming value is megahertz
						sscanf(seg, "%d", &br);
						chipdefs[id].bps2[b] = br;
						//LOGI("\t%s.bps2[%d] = %u", chipdefs[id].name, b, chipdefs[id].bps[b]);
					}
				}
			} else {
				LOGD("Ignoring '%s'", s);
			}
		}
	}

	fclose(F);
	F = NULL;

	return E_NONE;
}

int validate_mcu16_chipdef16() {
	for (int id = 0; id < MAX_MCU16_TYPE && chipdefs[id].name[0]; id++) {
		//Check if Baud2 is valid. If not, use a default_bps_series[].
		for (int i = 0; chipdefs[id].bps[i] > 0; i++) {
			if (chipdefs[id].bps2[i] == 0) {
				chipdefs[id].bps2[i] = default_bps_series[i];
				LOGD("Fixing '%s' bps2[%d] = %d", chipdefs[id].name, i, default_bps_series[i]);
			}
		}
	}

	return E_NONE;
}


/** License clause. */
const char *license = "\
\n\
Copyright (C) 2014-2016  Kari Sigurjonsson\n\
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
Usage: ./kuji16 -m<mcu> -p<com> [-c<freq>] [-r<file>] [-e] [-v<verbosity>] [-d] [-q] [-w<file>]\n\
  -h         Print help and exit.\n\
  -H         Print all supported MCUs and exit.\n\
  -V         Print application version and exit.\n\
  -d         Hex-dump communication to stdout.\n\
  -v<verbo>  Select verbosity level: 0-none, 1-errors, 2-warnings, 3-info, 4-debug.\n\
  -l<file>   Write log to <file> instead of main.log.\n\
  -p<com>    Set com port Id from 1-99 on Windows or com port device e.g. '/dev/ttyS0' on Linux.\n\
  -m<mcu>    Select MCU by name e.g. 'mb90f598g'. Case-insensitive.\n\
  -c<freq>   Select target crystal (megahertz) e.g 4, 8, 16 etc. Default is 4 Mhz.\n\
  -t<sec>   Select discovery timeout in seconds. Default is 15 seconds.\n\
  -b         Blank-check and exit immediately after.\n\
  -r<file>   Read MCU flash and write it as S-Records to file.\n\
  -e         Erase MCU flash.\n\
  -w<file>   Write S-Record file to MCU flash.\n\
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

int process_params16(int argc, char *argv[], struct params16 *params) {
	int id;
	int opt;

	memset(params, 0x00, sizeof(struct params16));
	params->argstr = "hHVdt:l:v:p:m:c:ber:w:";

	while ((opt = getopt(argc, argv, params->argstr)) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				return 1;

			case 'H':
				LOGI("\nSupported MCUs:");
				for (id = 0; id < MAX_MCU16_TYPE && chipdefs[id].name[0]; id++) {
					LOGI("%s", chipdefs[id].name);
				}
				LOGI("--------------------------");
				return 1;

			case 'V':
				fprintf(stderr, "%s\n%s\n", version_string(), license);
				return 1;

			case 'd':
				params->debugging = true;
				break;

			case 't':
				params->timeoutsec = strtoint32(optarg, 10, NULL);
				break;

			case 'v':
				verbosity = strtoint32(optarg, 10, NULL);
				break;

			case 'l':
				if (optarg) {
					loggpath = optarg;
					if (flogg) {
						fclose(flogg);
						flogg = NULL;
					}
				}
				LOGD("Using '%s' for log.", loggpath);
				break;

			case 'p':
				params->comarg = optarg;
				break;

			case 'm':
				if (optarg) {
					id = find_mcu16_by_name(optarg);
					if (id) {
						params->chip = &chipdefs[id];
					} else {
						LOGE("ERROR: Invalid option '%s' to -m. This is not a MCU name.", optarg);
						return FAIL_ARGUMENT;
					}
				}
				break;

			case 'c':
				if (optarg) {
					id = strtoint32(optarg, 10, NULL);
					id *= 1000000;
					for (int i = 1; i < N_FREQUENCY; i++) {
						if (frequencies[i] == id) {
							params->freq = frequencies[i];
						}
					}
				}
				break;

			case 'b':
				params->blankcheck = true;
				break;

			case 'r':
				if (optarg && optarg[0]) {
					params->savepath = optarg;
					params->read = true;
				}
				break;

			case 'e':
				params->erase = true;
				break;

			case 'w':
				if (optarg && optarg[0]) {
					params->srecpath = optarg;
					params->write = true;
				}
				break;

			case '?':
				LOGE("Argument error!");
				return FAIL_ARGUMENT;
		}
	}

	if (params->comarg == NULL) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return FAIL_ARGUMENT;
	}

	if (params->timeoutsec < 1 || params->timeoutsec > 60) {
		if (params->timeoutsec != 0) LOGW("Invalid timeout, setting default 15 seconds.");
		params->timeoutsec = 15;
	}

	if (params->chip == NULL) {
		LOGE("ERROR: Missing or invalid option '-m'.\n");
		print_help();
		return FAIL_ARGUMENT;
	}

	//Morph frequency into MCU specific index that we select bps by.
	//This is because serial port bitrate follows clock selection.
	if (params->freq <= 0) {
		params->freq = FREQ_4MHZ;
		LOGD("Using default frequency %dHz.", params->freq);
	}

	for (id = 0; id < N_FREQUENCY; id++) {
		if (params->chip->clock[id] == params->freq) {
			params->freqid = id;
			break;
		}
	}

	if (params->freqid < 0) {
		LOGE("The clock frequency '%d' is not supported on MCU '%s'.", params->freq, params->chip->name);
		return FAIL_ARGUMENT;
	}

	return E_NONE;
}

int process16(struct params16 *params) {
	int bps = 0;
	int id = 0;
	int rc;
	int bytes = 0;
	uint16_t csum = 0;
	struct serial serial;

	char compath[256];

	bool isblank;

#ifdef __WIN32__
	int comid = 0;
#endif

	memset(compath, 0x00, sizeof(compath));

	//NOTE, bps follows the clock, this is configured in 'chipdef16.ini'.
	bps = params->chip->bps[params->freqid];

	if (bps <= 0) {
		LOGE("Internal error: Invalid bitrate.");
		return FAIL_ARGUMENT;
	}

#ifdef __WIN32__
	comid = strtoint32(params->comarg, 10, &rc);
	if (rc != E_NONE || comid <= 0 || comid >= 100) {
		LOGE("Missing or invalid option '-p'.");
		print_help();
		return FAIL_ARGUMENT;
	}
	snprintf(compath, sizeof(compath) - 1, "\\\\.\\COM%d:%d:8N1", comid, params->chip->bps[params->freqid]);
#else
	snprintf(compath, sizeof(compath) - 1, "%s:%d:8N1", params->comarg, params->chip->bps[params->freqid]);
#endif

	if (params->write && params->srecpath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-w'.");
		print_help();
		return FAIL_ARGUMENT;
	}

	if (params->read && params->savepath == NULL) {
		LOGE("ERROR: Missing or malformed option to '-r'.");
		print_help();
		return FAIL_ARGUMENT;
	}

	//Open up serial port for birom.
	memset(&serial, 0x00, sizeof(struct serial));
	//LOGD("Compath: '%s'", compath);
	rc = serial_open(&serial, compath);
	if (rc != E_NONE) {
		LOGE("Error opening serial port.");
		serial_close(&serial);
		return FAIL_SERIAL;
	}

	serial.debug = params->debugging;

	/****************************************************************************
	  Stage 1 BIROM (Built-In-ROM).
	 ****************************************************************************/
	struct birom16_state *birom = NULL;

	LOGD("---------- BIROM16 START ----------");

	rc = birom16_new(&birom, params->chip, &serial);
	if (rc != E_NONE) {
		birom16_free(&birom);
		serial_close(&serial);
		return FAIL_INITBIROM;
	}

	birom->serial->debug = params->debugging;

	//First handshake is to give user chance to power up MCU.
	//FIXME LOGI("Probing for MCU. Please apply power to board...");

	//Is the audience listening?
	rc = birom16_connect(birom, params->timeoutsec);
	if (rc != E_NONE) {
		birom16_free(&birom);
		serial_close(&serial);
		return FAIL_TIMEOUT;
	}

	LOGI("Writing KERNAL to iRAM...");

	//Relax guy.
	msleep(500);

	//Dump stage 2 binary into MCU RAM.
	rc = birom16_write(birom, params->chip->address_load, birom->kernaldata, birom->kernalsize);
	if (rc != E_NONE) {
		birom16_free(&birom);
		serial_close(&serial);
		return FAIL_WRITE;
	}

	//Relax guy.
	msleep(500);

	//Check if birom is still there.
	rc = birom16_connect(birom, params->timeoutsec);
	if (rc != E_NONE) {
		birom16_free(&birom);
		serial_close(&serial);
		return FAIL_TIMEOUT;
	}

	LOGI("Booting KERNAL in iRAM...");

	//Relax guy.
	msleep(500);

	// Call stage 1 and transfer control to stage 2 kernal16.
	rc = birom16_call(birom, params->chip->address_load);
	if (rc != E_NONE) {
		birom16_free(&birom);
		serial_close(&serial);
		return FAIL_TIMEOUT;
	}

	birom16_free(&birom);

	LOGD("---------- BIROM16 DONE ----------\n");

	//Relax guy.
	msleep(500);

	//Adjust baudrate to suit stage 2.
	rc = serial_setbaud(&serial, params->chip->bps2[params->freqid] > 0 ? params->chip->bps2[params->freqid] : 9600);
	if (rc != E_NONE) {
		LOGE("Error setting baudrate.");
		serial_close(&serial);
		return FAIL_SERIAL;
	}

	/****************************************************************************
	  Stage 2 KERNAL
	 ****************************************************************************/
	struct kernal16 *kernal = NULL;

	LOGD("========== KERNAL16 START ==========");

	rc = kernal16_new(&kernal, params->chip, &serial);
	if (rc != E_NONE) {
		kernal16_free(&kernal);
		serial_close(&serial);
		return FAIL_INITKERNAL;
	}

	kernal->serial->debug = params->debugging;

	//Is the audience listening?
	rc = kernal16_intro(kernal);
	if (rc != E_NONE) {
		kernal16_free(&kernal);
		LOGE("ERROR: Processor not ready!\nPlease disable watchdog and check connections.");
		serial_close(&serial);
		return FAIL_TIMEOUT;
	}

	LOGI("Blank checking...");

	//Relax guy.
	msleep(500);

	//Always blank-check.
	rc = kernal16_blankcheck(kernal, params->chip->flash_start);
	if (rc < 0) {
		LOGE("ERROR: Could not perform blank check!");
		kernal16_free(&kernal);
		serial_close(&serial);
		return FAIL_BLANK;
	}
	isblank = (rc == 1);

	//Relax guy.
	msleep(500);

	//Exit early if chip is full and there are no operations or
	//if chip is empty and operations would not be possible.
	if (
		(params->blankcheck)
		|| (isblank && (!params->write))
		|| ((isblank && (params->read || params->erase) && !params->write))
		|| (params->read && isblank)
		|| (!isblank && (!params->write && !params->read && !params->erase))
	) {
		LOGI("== Chip Is %s ==", (isblank) ? "Blank" : "Not Blank");
		LOGD("========== KERNAL16 DONE ==========");
		kernal16_free(&kernal);
		serial_close(&serial);
		return isblank ? FAIL_ISBLANK : FAIL_NOTBLANK;
	}

	//Relax guy.
	msleep(500);

	//Read flash contents
	if (params->read) {
		if (isblank) {
			LOGI("== Chip Is Blank ==");
			LOGD("========== KERNAL16 DONE ==========");
			kernal16_free(&kernal);
			serial_close(&serial);
			return FAIL_ISBLANK;
		}

		//Sector by sector!
		uint8_t *buff = calloc(1, params->chip->flash_size * 2);
		assert(buff);

		bytes = 0;
		csum = 0;

		LOGR("[INF]: Reading ");
		for (uint32_t addr = params->chip->flash_start; addr < params->chip->flash_start + params->chip->flash_size; addr += 0x200, bytes += 512) {
			rc = kernal16_readflash(kernal, addr, buff + bytes, 512, &csum);
			if (rc != E_NONE) {
				LOGE("Error receiving flash contents.");
				kernal16_free(&kernal);
				serial_close(&serial);
				return FAIL_READ;
			}

//#ifdef DEBUGGING
#ifdef __WIN32__
			LOGR("Receiving 512 bytes from sector 0x%06X, last CRC16 0x%04X", addr, csum);
#else
			LOGR("\rReceiving 512 bytes from sector 0x%06X, last CRC16 0x%04X", addr, csum);
#endif
//#else
//			if (bytes % 4096 == 0) {
//				LOGR("#");
//			}
//#endif
		}

#ifndef __WIN32__
		LOGR("\n");
#endif

		rc = srec_writefilebin(buff, bytes, params->savepath, 2, params->chip->flash_start);
		if (rc != E_NONE) {
			LOGE("Error serializing S-Records.");
			kernal16_free(&kernal);
			serial_close(&serial);
			return FAIL_SRECORD;
		}

		free(buff);
		buff = NULL;

		LOGI("== Chip Read Successfully ==");
	}

	//Relax guy.
	msleep(500);

	//Return early is chip is full and trying to write without erasing first.
	if (!isblank && params->write && !params->erase) {
		LOGE("Error: Trying to write into an already full MCU. Did you forget to add '-e' argument?");
		LOGD("========== KERNAL16 DONE ==========");
		kernal16_free(&kernal);
		serial_close(&serial);
		return FAIL_NOTBLANK;
	}

	//Relax guy.
	msleep(500);

	//Erase chip.
	if (params->erase && !isblank) {

		//Delay for 5 seconds to make sure that the watchdog will not bark in the
		//middle of an erasure as that bricks the MCU.
		for (int i = 5; i > 0; i--) {
			LOGR("Erasing starts after %d seconds...\n!!DO NOT UNPLUG POWER DURING ERASURE !!", i);
			msleep(1000);
		}

		LOGR("[INF]: Erasing ");
		rc = kernal16_erasechip(kernal, params->chip->flash_start);
		if (rc != E_NONE) {
			LOGR("\n");
			LOGE("ERROR: Could not erase flash!");
			kernal16_free(&kernal);
			serial_close(&serial);
			return FAIL_ERASE;
		}
		LOGR("\n");
		LOGI("== Chip Erased ==");
	}

	//Relax guy.
	msleep(500);

	//Program S-Record.
	if (params->write) {
		uint8_t *buf = NULL;

		//Copy flash data from S-Records in file into a linear buffer.
		//The buffer is already 2^24 bytes so we can index it directly from chip->flash_start to chip->flash_end inclusively.
		rc = srec_readfilebin(&buf, params->srecpath, params->chip->flash_start, params->chip->flash_end);
		if (rc != E_NONE || buf == NULL) {
			LOGE("ERROR: Could not interpret S-Records from file '%s'.", params->srecpath);
			kernal16_free(&kernal);
			serial_close(&serial);
			return FAIL_SRECORD;
		}

		LOGD("Loaded S-Records from '%s'.", params->srecpath);

		bytes = 0;

		//Write out linear buffer into MCU flash in 512 byte chunks.
		LOGD("Writing 0x%06X bytes...", params->chip->flash_size);
		LOGR("[INF]: Writing ");
		for (uint32_t addr = params->chip->flash_start; addr < params->chip->flash_end; addr += 512, bytes += 512) {
			if (isflashbufempty(buf + addr, 512) == false) {
				rc = kernal16_writeflash(kernal, addr, buf + addr, 512);
				if (rc != E_NONE) {
					kernal16_free(&kernal);
					serial_close(&serial);
					return FAIL_WRITE;
				}

#ifdef __WIN32__
				LOGI("Writing sector 0x%06X", addr);
#else
				LOGR("\rWriting sector 0x%06X", addr);
#endif

//				if (bytes % 4096 == 0) {
//					LOGR("#");
//				}
			}
		}
		LOGR("\n");

		free(buf);
		buf = NULL;

		LOGI("== Chip Programmed Successfully ==");

	}

	kernal16_free(&kernal);

	LOGD("========== KERNAL16 DONE ==========");

	serial_close(&serial);
	return E_NONE;
}

/** @} */
