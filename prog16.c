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

/** Name of the chip definition file for 16 bit MCUs. */
const char *chipdef_filename = "chipdef16.ini";

struct mcu16_tag mcu16_map[] = {
	{NULL,					MCU16_INVALID},
	{"MB90F334",			MCU16_MB90F334},
	{"MB90F335A",			MCU16_MB90F335A},
	{"MB90F337",			MCU16_MB90F337},
	{"MB90F342A/CA/AS/CAS",	MCU16_MB90F342A},
	{"MB90F342A",			MCU16_MB90F342A},
	{"MB90F342CA",			MCU16_MB90F342A},
	{"MB90F342AS",			MCU16_MB90F342A},
	{"MB90F342CAS",			MCU16_MB90F342A},
	{"MB90F342E/CE/ES/CES",	MCU16_MB90F342E},
	{"MB90F342E",			MCU16_MB90F342E},
	{"MB90F342CE",			MCU16_MB90F342E},
	{"MB90F342ES",			MCU16_MB90F342E},
	{"MB90F342CES",			MCU16_MB90F342E},
	{"MB90F345A/CA/AS/CAS",	MCU16_MB90F345A},
	{"MB90F345A",			MCU16_MB90F345A},
	{"MB90F345CA",			MCU16_MB90F345A},
	{"MB90F345AS",			MCU16_MB90F345A},
	{"MB90F345CAS",			MCU16_MB90F345A},
	{"MB90F345E/CE/ES/CES",	MCU16_MB90F345E},
	{"MB90F345E",			MCU16_MB90F345E},
	{"MB90F345CE",			MCU16_MB90F345E},
	{"MB90F345ES",			MCU16_MB90F345E},
	{"MB90F345CES",			MCU16_MB90F345E},
	{"MB90F346A/CA/AS/CAS",	MCU16_MB90F346A},
	{"MB90F346A",			MCU16_MB90F346A},
	{"MB90F346CA",			MCU16_MB90F346A},
	{"MB90F346AS",			MCU16_MB90F346A},
	{"MB90F346CAS",			MCU16_MB90F346A},
	{"MB90F346E/CE/ES/CES",	MCU16_MB90F346E},
	{"MB90F346E",			MCU16_MB90F346E},
	{"MB90F346CE",			MCU16_MB90F346E},
	{"MB90F346ES",			MCU16_MB90F346E},
	{"MB90F346CES",			MCU16_MB90F346E},
	{"MB90F347A/CA/AS/CAS",	MCU16_MB90F347A},
	{"MB90F347A",			MCU16_MB90F347A},
	{"MB90F347CA",			MCU16_MB90F347A},
	{"MB90F347AS",			MCU16_MB90F347A},
	{"MB90F347CAS",			MCU16_MB90F347A},
	{"MB90F347E/CE/ES/CES",	MCU16_MB90F347E},
	{"MB90F347E",			MCU16_MB90F347E},
	{"MB90F347CE",			MCU16_MB90F347E},
	{"MB90F347ES",			MCU16_MB90F347E},
	{"MB90F347CES",			MCU16_MB90F347E},
	{"MB90F349A/CA/AS/CAS",	MCU16_MB90F349A},
	{"MB90F349A",			MCU16_MB90F349A},
	{"MB90F349CA",			MCU16_MB90F349A},
	{"MB90F349AS",			MCU16_MB90F349A},
	{"MB90F349CAS",			MCU16_MB90F349A},
	{"MB90F349E/CE/ES/CES",	MCU16_MB90F349E},
	{"MB90F349E",			MCU16_MB90F349E},
	{"MB90F349CE",			MCU16_MB90F349E},
	{"MB90F349ES",			MCU16_MB90F349E},
	{"MB90F349CES",			MCU16_MB90F349E},
	{"MB90F351/S",			MCU16_MB90F351S},
	{"MB90F351S",			MCU16_MB90F351S},
	{"MB90F351B/BS",		MCU16_MB90F351B},
	{"MB90F351B",			MCU16_MB90F351B},
	{"MB90F351BS",			MCU16_MB90F351B},
	{"MB90F351A/AS/TA/TAS",	MCU16_MB90F351A},
	{"MB90F351A",			MCU16_MB90F351A},
	{"MB90F351AS",			MCU16_MB90F351A},
	{"MB90F351TA",			MCU16_MB90F351A},
	{"MB90F351TAS",			MCU16_MB90F351A},
	{"MB90F351E/ES/TE/TES",	MCU16_MB90F351E},
	{"MB90F351E",			MCU16_MB90F351E},
	{"MB90F351ES",			MCU16_MB90F351E},
	{"MB90F351TE",			MCU16_MB90F351E},
	{"MB90F351TES",			MCU16_MB90F351E},
	{"MB90F352/S",			MCU16_MB90F352S},
	{"MB90F352S",			MCU16_MB90F352S},
	{"MB90F352B/BS",		MCU16_MB90F352B},
	{"MB90F352B",			MCU16_MB90F352B},
	{"MB90F352BS",			MCU16_MB90F352B},
	{"MB90F352A/AS/TA/TAS",	MCU16_MB90F352A},
	{"MB90F352A",			MCU16_MB90F352A},
	{"MB90F352AS",			MCU16_MB90F352A},
	{"MB90F352TA",			MCU16_MB90F352A},
	{"MB90F352TAS",			MCU16_MB90F352A},
	{"MB90F352E/ES/TE/TES",	MCU16_MB90F352E},
	{"MB90F352E",			MCU16_MB90F352E},
	{"MB90F352ES",			MCU16_MB90F352E},
	{"MB90F352TE",			MCU16_MB90F352E},
	{"MB90F352TES",			MCU16_MB90F352E},
	{"MB90F357A/AS/TA/TAS",	MCU16_MB90F357A},
	{"MB90F357A",			MCU16_MB90F357A},
	{"MB90F357AS",			MCU16_MB90F357A},
	{"MB90F357TA",			MCU16_MB90F357A},
	{"MB90F357TAS",			MCU16_MB90F357A},
	{"MB90F357E/ES/TE/TES",	MCU16_MB90F357E},
	{"MB90F357E",			MCU16_MB90F357E},
	{"MB90F357ES",			MCU16_MB90F357E},
	{"MB90F357TE",			MCU16_MB90F357E},
	{"MB90F357TES",			MCU16_MB90F357E},
	{"MB90F362E/ES/TE/TES",	MCU16_MB90F362E},
	{"MB90F362E",			MCU16_MB90F362E},
	{"MB90F362ES",			MCU16_MB90F362E},
	{"MB90F362TE",			MCU16_MB90F362E},
	{"MB90F362TES",			MCU16_MB90F362E},
	{"MB90F367E/ES/TE/TES",	MCU16_MB90F367E},
	{"MB90F367E",			MCU16_MB90F367E},
	{"MB90F367ES",			MCU16_MB90F367E},
	{"MB90F367TE",			MCU16_MB90F367E},
	{"MB90F367TES",			MCU16_MB90F367E},
	{"MB90F372",			MCU16_MB90F372},
	{"MB90F378",			MCU16_MB90F378},
	{"MB90F387/S",			MCU16_MB90F387S},
	{"MB90F387S",			MCU16_MB90F387S},
	{"MB90F394H",			MCU16_MB90F394H},
	{"MB90F395H",			MCU16_MB90F395H},
	{"MB90MF408",			MCU16_MB90MF408},
	{"MB90F423GA/GC",		MCU16_MB90F423G},
	{"MB90F423GA",			MCU16_MB90F423G},
	{"MB90F423GC",			MCU16_MB90F423G},
	{"MB90F428GA/GC",		MCU16_MB90F428G},
	{"MB90F428GA",			MCU16_MB90F428G},
	{"MB90F428GC",			MCU16_MB90F428G},
	{"MB90F438L/LS",		MCU16_MB90F438L},
	{"MB90F438L",			MCU16_MB90F438L},
	{"MB90F438LS",			MCU16_MB90F438L},
	{"MB90F439/S",			MCU16_MB90F439S},
	{"MB90F439S",			MCU16_MB90F439S},
	{"MB90F443G",			MCU16_MB90F443G},
	{"MB90F455/S",			MCU16_MB90F455S},
	{"MB90F455S",			MCU16_MB90F455S},
	{"MB90F456/S",			MCU16_MB90F456S},
	{"MB90F456S",			MCU16_MB90F456S},
	{"MB90F457/S",			MCU16_MB90F457S},
	{"MB90F457S",			MCU16_MB90F457S},
	{"MB90F462",			MCU16_MB90F462},
	{"MB90F474H/L",			MCU16_MB90F474H},
	{"MB90F474H",			MCU16_MB90F474H},
	{"MB90F474L",			MCU16_MB90F474H},
	{"MB90F476/A",			MCU16_MB90F476A},
	{"MB90F476A",			MCU16_MB90F476A},
	{"MB90F481",			MCU16_MB90F481},
	{"MB90F482",			MCU16_MB90F482},
	{"MB90F488",			MCU16_MB90F488},
	{"MB90F489",			MCU16_MB90F489},
	{"MB90F497/G",			MCU16_MB90F497G},
	{"MB90F497G",			MCU16_MB90F497G},
	{"MB90F498G",			MCU16_MB90F498G},
	{"MB90F523B",			MCU16_MB90F523B},
	{"MB90F543/G/GS",		MCU16_MB90F543G},
	{"MB90F543G",			MCU16_MB90F543G},
	{"MB90F543GS",			MCU16_MB90F543G},
	{"MB90F546G/GS",		MCU16_MB90F546G},
	{"MB90F546G",			MCU16_MB90F546G},
	{"MB90F546GS",			MCU16_MB90F546G},
	{"MB90F548G/GS",		MCU16_MB90F548G},
	{"MB90F548G",			MCU16_MB90F548G},
	{"MB90F548GS",			MCU16_MB90F548G},
	{"MB90F549/G/GS",		MCU16_MB90F549G},
	{"MB90F549G",			MCU16_MB90F549G},
	{"MB90F549GS",			MCU16_MB90F549G},
	{"MB90F553A",			MCU16_MB90F553A},
	{"MB90F562/B",			MCU16_MB90F562B},
	{"MB90F562B",			MCU16_MB90F562B},
	{"MB90F568",			MCU16_MB90F568},
	{"MB90F574/A",			MCU16_MB90F574A},
	{"MB90F574A",			MCU16_MB90F574A},
	{"MB90F583B/C/CA",		MCU16_MB90F583B},
	{"MB90F583B",			MCU16_MB90F583B},
	{"MB90F583C",			MCU16_MB90F583B},
	{"MB90F583CA",			MCU16_MB90F583B},
	{"MB90F584C/CA",		MCU16_MB90F584C},
	{"MB90F584C",			MCU16_MB90F584C},
	{"MB90F584CA",			MCU16_MB90F584C},
	{"MB90F591A/G",			MCU16_MB90F591A},
	{"MB90F591A",			MCU16_MB90F591A},
	{"MB90F591G",			MCU16_MB90F591A},
	{"MB90F594A/G",			MCU16_MB90F594A},
	{"MB90F594A",			MCU16_MB90F594A},
	{"MB90F594G",			MCU16_MB90F594A},
	{"MB90F598/G",			MCU16_MB90F598G},
	{"MB90F598G",			MCU16_MB90F598G},
	{"MB90F654A",			MCU16_MB90F654A},
	{"MB90F803",			MCU16_MB90F803},
	{"MB90F804",			MCU16_MB90F804},
	{"MB90F822",			MCU16_MB90F822},
	{"MB90F823",			MCU16_MB90F823},
	{"MB90F828B",			MCU16_MB90F828B},
	{"MB90F867A/AS",		MCU16_MB90F867A},
	{"MB90F867A",			MCU16_MB90F867A},
	{"MB90F867AS",			MCU16_MB90F867A},
	{"MB90F867E/ES",		MCU16_MB90F867E},
	{"MB90F867E",			MCU16_MB90F867E},
	{"MB90F867ES",			MCU16_MB90F867E},
	{"MB90F882",			MCU16_MB90F882},
	{"MB90F883/A/B",		MCU16_MB90F883A},
	{"MB90F883A",			MCU16_MB90F883A},
	{"MB90F883B",			MCU16_MB90F883A},
	{"MB90F884/A/B/C",		MCU16_MB90F884A},
	{"MB90F884A",			MCU16_MB90F884A},
	{"MB90F884B",			MCU16_MB90F884A},
	{"MB90F884C",			MCU16_MB90F884A},
	{"MB90F897/S",			MCU16_MB90F897S},
	{"MB90F897S",			MCU16_MB90F897S},
	{"MB90F912",			MCU16_MB90F912},
	{"MB90F922",			MCU16_MB90F922},
	{"MB90F923",			MCU16_MB90F923},
	{"MB90F924",			MCU16_MB90F924},
	{"MB90F946A",			MCU16_MB90F946A},
	{"MB90F947A",			MCU16_MB90F947A},
	{"MB90F949A",			MCU16_MB90F949A},
	{"MB90F952",			MCU16_MB90F952},
	{"MB90F962",			MCU16_MB90F962},
	{"MB90F983",			MCU16_MB90F983},
	{"MB90F997",			MCU16_MB90F997},
	//--
	{NULL,					MCU16_INVALID}
};

int find_mcu16_by_name(char *s) {
	int rc = 0;
	for (int i = 1; mcu16_map[i].name; i++) {
		if ((rc = strcasecmp(mcu16_map[i].name, s)) == 0) {
			return mcu16_map[i].type;
		}
	}
	return E_NOTFOUND;
}

const char *mcu16_name(enum mcu16_type type) {
	for (int i = 1; mcu16_map[i].name; i++) {
		if (mcu16_map[i].type == type) {
			return mcu16_map[i].name;
		}
	}
	return NULL;
}

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
	Holds configuration for each 16 bit MCU type.
	This array is initialized with data from 'chipdef16.ini'.
*/
struct chipdef16 chipdefs[MAX_MCU16_TYPE];

int process_chipdef() {
	FILE *F = NULL;
	char *s;
	char line[256];
	char key[256];
	char value[256];
	int id = 0;
	int rc;

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
			id = find_mcu16_by_name(s);
			if (id) {
				chipdefs[id].mcu = id;
				strncpy(chipdefs[id].name, s, sizeof(chipdefs[id].name) - 1);
			} else {
				LOGE("Unknown chipdef heading: '%s'", s);
				strncpy(chipdefs[id].name, "Unknown", sizeof(chipdefs[id].name) - 1);
			}
		} else {
			if (id) {
				if (sscanf(s, "%[^=]=%s", key, value) < 2) {
					LOGE("Invalid chipdef line: '%s'.", s);
					continue;
				}

				if (strcasecmp(key, "DownloadFile") == 0) {
					//Base name of the kernal file aka stage 2 boot loader.
					strncpy(chipdefs[id].kernal, value, sizeof(chipdefs[id].kernal) - 1);
					//LOGD("\t%s.kernal = %s", mcu16_name(id), chipdefs[id].kernal);
				} else if (strcasecmp(key, "LoadAddress") == 0) {
					//Where in RAM to store stage 2 binary. This is usually 0x0990 or 0x0190.
					chipdefs[id].address_load = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.address_load = 0x%X", mcu16_name(id), chipdefs[id].address_load);
				} else if (strcasecmp(key, "StartAddress") == 0) {
					//uint16_t flash_start;			/**< Where in FLASH to store user firmware. */
					chipdefs[id].flash_start = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_start = 0x%X", mcu16_name(id), chipdefs[id].flash_start);
				} else if (strcasecmp(key, "EndAddress") == 0) {
					//uint16_t flash_end;				/**< Last address of flash. */
					chipdefs[id].flash_end = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_end = 0x%X", mcu16_name(id), chipdefs[id].flash_end);
				} else if (strcasecmp(key, "FlashSize") == 0) {
					//uint16_t flash_size;			/**< Flash size (flash_end - flash_start). */
					chipdefs[id].flash_size = strtoint32(value, 16, &rc);
					if (rc != E_NONE) {
						LOGE("Conversion error on entry '%s' = '%s' is not a hexadecimal number.", key, value);
						continue;
					}
					//LOGD("\t%s.flash_size = 0x%X", mcu16_name(id), chipdefs[id].flash_size);
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
						LOGD("\t%s.clock[%d] = %u", mcu16_name(id), c, chipdefs[id].clock[c]);
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
						//LOGD("\t%s.bps[%d] = %u", mcu16_name(id), b, chipdefs[id].bps[b]);
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
						//LOGD("\t%s.bps[%d] = %u", mcu16_name(id), b, chipdefs[id].bps[b]);
					}
				}
			} else {
				//LOGD("Ignoring '%s'", s);
			}
		}
	}

	fclose(F);
	F = NULL;

	return E_NONE;
}

/** @} */
