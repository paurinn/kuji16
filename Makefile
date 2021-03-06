#Kuji16 Flash MCU Programmer
#Copyright (C) 2014-2016 Kari Sigurjonsson

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

###############################################################################

#To get a fully verbose make output do declare 'AT' at the command line like so: 'make AT='.
#By default AT is undefined and thus assigned the string '@'.
AT ?= @

# Attach debugging symbols and activate debug printout.
DEBUGGING = false

# Base name of the binary executable. Version numbers are appended to final binary name.
OUTPUT = kuji16

# Build graphical user interface on platforms that support it.
GUI = true

# Build a version where reading is inhibited unless uer
# supplies command line argument "pleaseenablereading".
READINGINHIBITED = true

# Any platform neutral options to the compiler.
# Platform specific stuff is in makefile.$(shell uname -s).
CFLAGS = -Wall -Wextra -Werror -Wfatal-errors -Wno-unused-parameter -Wno-unused-variable --std=gnu99 -I./include

# Any platform neutral options to the linker.
#LDFLAGS =

#RULES += <rules_list>		# List of make rules to run but after the default.

#AFTER_INSTALL	+= <shell commands> # Command line to execute as the last step in the install directive.

# List of source files to compile.
SRCS =	\
		stdafx.c \
		errorcode.c \
		version.c \
		log.c \
		util.c \
		serial.c \
		srec.c \
		prog16.c \
		birom16.c \
		kernal16.c

###############################################################################

#Version is stored in the file VERSION.
MAJOR	= $(shell sed -n '1p' VERSION | cut -d= -f2)
MINOR	= $(shell sed -n '2p' VERSION | cut -d= -f2)
BUILD	= $(shell sed -n '3p' VERSION | cut -d= -f2)
COMPANY	= $(shell sed -n '4p' VERSION | cut -d= -f2)

REVISION = $(shell svn info 2>/dev/null | grep '^Revision' | sed -e 's/Revision: *//')
ifeq "$(REVISION)" ""
REVISION = 0
endif

CFLAGS += -DREVISION=$(REVISION) -DBUILD_TIMESTAMP=$(shell date '+%s') -DMAJORVERSION=$(MAJOR) -DMINORVERSION=$(MINOR) -DBUILD=$(BUILD)

# Select debugging or optimized compiler flags.
ifeq ($(DEBUGGING), true)
CFLAGS += -O0 -g -DDEBUGGING
else
CFLAGS += -O3
endif

ifeq ($(GUI), true)
CFLAGS += -DGUI
endif

# Set reading inhibit mambo.
ifeq ($(READINGINHIBITED), true)
CFLAGS += -DREADINGINHIBITED
endif

RCOBJ = kuji16_rc.o

INSTALLDIR = $(PREFIX)/bin

#Stage directory is used to build a zip package of the final binary + documentation.
#This can be overwritten by platform specific makefile.
STAGEDIR = $(OUTPUT)_stage

#Name of zipped file with executable and documentation.
ZIPOUT = $(OUTPUT).$(MAJOR).$(MINOR).$(BUILD).zip

CLEANFILES += $(OUTPUT)$(EXT) *.core gmon.out $(OBJS) $(OUTPUT).sha1 $(EXTRACLEAN) doc/*.tmp *.tmp $(RCOBJ)
MRPROPERFILES += $(CLEANFILES) doc/latex *.log
DISTCLEANFILES += $(MRPROPERFILES) html $(STAGEDIR) $(ZIPOUT)

STAGEFILES += $(OUTPUT)$(EXT) kernal16/ chipdef16.ini LICENSE README HISTORY

#Include system specific Makefile. This is based on kernel name from 'uname -s'.
#The basic declarations can be overwritten to suit each system.
#You can override this on the command line: `make PLAT=MINGW32_NT-6.1`.
#PLAT?=$(shell uname -s)
PLAT=MINGW32_NT-6.1
TOP := $(dir $(lastword $(MAKEFILE_LIST)))
include $(TOP)/makefile.$(PLAT)

### Rules
.SUFFIXES : .c .o
.PHONY: info doc clean mrproper dist distclean help prep

RULES += $(OBJS) $(OUTPUT)$(EXT)

OBJS = $(SRCS:.c=.o)

all: $(RULES)

.c.o:
	$(ECHO) "[COMPILE] $@"
	$(AT)$(CC) $(CFLAGS) -c -o $*.o $<

#Special target for compiled windows resource file.
$(RCOBJ): $(RCFILE)
	$(ECHO) "[$(FG_CYAN)WINDRES$(NORMAL)] $(RCFILE) > $(RCOBJ)"
	$(AT)$(RESCC) $(RESCFLAGS) $(RCFILE) $(RCOBJ)

$(OUTPUT)$(EXT): $(RCOBJ) $(OBJS)
	$(ECHO) "[LINKING] $(OUTPUT)$(EXT)"
	$(AT)$(LD) $(OBJS) $(LDFLAGS) -o $(OUTPUT)$(EXT)

info:
	$(ECHO) "Source to build for $(OUTPUT):"
	$(AT)ls -1lh $(SRCS)
	$(ECHO) ""
	$(ECHO) "Files included in $(ZIPOUT):"
	$(ECHO) $(STAGEFILES)

doc:
	$(ECHO) "[DOXYGEN] doc"
	$(AT)doxygen doc/Doxyfile

clean:
	$(ECHO) "[CLEAN] $(CLEANFILES)"
	$(AT)$(RM) -rf $(CLEANFILES)

mrproper:
	$(ECHO) "[MRPROPER] $(MRPROPERFILES)"
	$(AT)$(RM) -rf $(MRPROPERFILES)

cloc:
	$(AT)cloc .

buildcounter:
	$(AT)lua buildcounter.lua

distclean:
	$(ECHO) "[DISTCLEAN] $(DISTCLEANFILES)"
	$(AT)$(RM) -rf $(DISTCLEANFILES)

dist: distclean $(RCOBJ) $(OUTPUT)$(EXT)
	$(AT)mkdir -p $(STAGEDIR)
	$(AT)$(CP) -r $(STAGEFILES) $(STAGEDIR)
	$(ECHO) "[ZIP] $(STAGEFILES) > $(ZIPOUT)"
	$(AT)$(ZIP) -qr9 $(ZIPOUT) $(STAGEDIR)

prep:
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FMC16LX/m_flash.* kernal16/
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FMC16LX/CHIPDEF.INI chipdef16.ini

help:
	$(AT)$(CAT) README

