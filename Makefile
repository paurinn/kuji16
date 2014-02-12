#Kuji16 Flash MCU Programmer
#Copyright (C) 2014 Kari Sigurjonsson

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

# Master Makefile.

#To get a fully verbose make output do declare 'AT' at the command line like so: 'make AT='.
#By default AT is undefined and thus assigned the string '@'.
AT ?= @

# Attach debugging symbols and activate debug printout.
DEBUGGING	= false

# Base name of the binary executable. Version numbers are appended to final binary name.
OUTPUT		= kuji16

#Build a static binary.
STATIC = true

# Any platform neutral options to the compiler.
# Platform specific stuff is in makefile.$(shell uname -s).
CFLAGS		= -Wall -Wextra -Werror -Wfatal-errors -Wno-unused-parameter -Wno-unused-but-set-variable --std=gnu99 -I./include -DCONSOLE

# Any platform neutral options to the linker.
#LDFLAGS	=

#RULES		+= <rules_list>		# List of make rules to run but after the default.

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
		kernal16.c \
		main16.c

#############################################################################

COLORIZE=true
ifeq ($(COLORIZE), true)
NORMAL	= \033[m

FG_BLACK		= \033[30m
FG_DKRED		= \033[31m
FG_DKGREEN		= \033[32m
FG_DKYELLOW		= \033[33m
FG_DKBLUE		= \033[34m
FG_DKMAGENTA	= \033[35m
FG_DKCYAN		= \033[36m
FG_GRAY			= \033[37m
FG_DKGRAY		= \033[30;1m
FG_RED			= \033[31;1m
FG_GREEN		= \033[32;1m
FG_YELLOW		= \033[33;1m
FG_BLUE			= \033[34;1m
FG_MAGENTA		= \033[35;1m
FG_CYAN			= \033[36;1m
FG_WHITE		= \033[37;1m

BG_BLACK		= \033[40m
BG_DKRED		= \033[41m
BG_DKGREEN		= \033[42m
BG_DKYELLOW		= \033[43m
BG_DKBLUE		= \033[44m
BG_DKMAGENTA	= \033[45m
BG_DKCYAN		= \033[46m
BG_GRAY			= \033[47m
BG_DKGRAY		= \033[40;1m
BG_RED			= \033[41;1m
BG_GREEN		= \033[42;1m
BG_YELLOW		= \033[43;1m
BG_BLUE			= \033[44;1m
BG_MAGENTA		= \033[45;1m
BG_CYAN			= \033[46;1m
BG_WHITE		= \033[47;1m
endif

#Version is stored in the file VERSION.
MAJOR	= $(shell sed -n '1p' VERSION | cut -d= -f2)
MINOR	= $(shell sed -n '2p' VERSION | cut -d= -f2)
BUILD	= $(shell sed -n '3p' VERSION | cut -d= -f2)
COMPANY	= $(shell sed -n '4p' VERSION | cut -d= -f2)

SVNREVISION = $(shell svn info 2>/dev/null | grep '^Revision' | sed -e 's/Revision: *//')
ifeq "$(SVNREVISION)" ""
SVNREVISION = 0
endif

CFLAGS += -DREVISION=$(SVNREVISION) -DBUILD_TIMESTAMP=$(shell date '+%s') -DMAJORVERSION=$(MAJOR) -DMINORVERSION=$(MINOR) -DBUILD=$(BUILD)

# Select static or dynamic binary.
ifeq ($(STATIC), true)
CFLAGS += -static -static-libgcc
LDFLAGS += -Wl,-static
endif

# Select debugging or optimized compiler flags.
ifeq ($(DEBUGGING), true)
CFLAGS += -O0 -g3 -ggdb3 -DDEBUGGING
else
CFLAGS += -O3
endif

INSTALLDIR = $(PREFIX)/bin

#Stage directory is used to build a zip package of the final binary + documentation.
STAGEDIR = $(OUTPUT)

#Name of zipped file with executable and documentation.
ZIPOUT = $(OUTPUT).$(MAJOR).$(MINOR).zip

CLEANFILES += $(OUTPUT)$(EXT) *.core gmon.out $(OBJS) $(OUTPUT).sha1 $(EXTRACLEAN) doc/*.tmp *.tmp
MRPROPERFILES += $(CLEANFILES) doc/latex *.log
DISTCLEANFILES += $(MRPROPERFILES) html $(STAGEDIR) $(ZIPOUT)

STAGEFILES += $(OUTPUT)$(EXT) html include chipdef16.ini kernal16 LICENSE README

#Include system specific Makefile. This is based on kernel name from 'uname -s'.
#The basic declarations can be overwritten to suit each system.
KERNEL=$(shell uname -s)
TOP := $(dir $(lastword $(MAKEFILE_LIST)))
include $(TOP)/makefile.$(KERNEL)

### Rules
.SUFFIXES : .c .o
.PHONY: info doc clean mrproper dist distclean help prep

RULES += $(OBJS) $(OUTPUT)$(EXT)

OBJS = $(SRCS:.c=.o)

all: $(RULES)

.c.o:
	$(ECHO) "[$(FG_CYAN)COMPILE$(NORMAL)] $@"
	$(AT)$(CC) $(CFLAGS) -c -o $*.o $<

$(OUTPUT)$(EXT): $(OBJS)
	$(ECHO) "[$(FG_GREEN)LINKING$(NORMAL)] $(OUTPUT)$(EXT)"
	$(AT)$(LD) $(OBJS) $(LDFLAGS) -o $(OUTPUT)$(EXT)

info:
	$(ECHO) "$(FG_BLUE)Source to build for $(FG_GREEN)$(OUTPUT)$(NORMAL):"
	$(AT)ls -1lh $(SRCS)
	$(ECHO) ""
	$(ECHO) "$(FG_BLUE)Files included in $(FG_GREEN)$(ZIPOUT)$(NORMAL):"
	$(ECHO) $(STAGEFILES)

doc:
	$(ECHO) "[$(FG_MAGENTA)DOXYGEN$(NORMAL)] doc"
	$(AT)doxygen doc/Doxyfile

clean:
	$(ECHO) "[$(FG_DKYELLOW)CLEAN$(NORMAL)] $(CLEANFILES)"
	$(AT)$(RM) -rf $(CLEANFILES)

mrproper:
	$(ECHO) "[$(FG_YELLOW)MRPROPER$(NORMAL)] $(MRPROPERFILES)"
	$(AT)$(RM) -rf $(MRPROPERFILES)

buildcounter:
	$(AT)./buildcounter.lua

distclean:
	$(ECHO) "[$(FG_YELLOW)DISTCLEAN$(NORMAL)] $(DISTCLEANFILES)"
	$(AT)$(RM) -rf $(DISTCLEANFILES)

dist: buildcounter distclean $(RCOBJ) $(OUTPUT)$(EXT) doc
	$(AT)$(MKDIR) -p $(STAGEDIR)/source
	$(AT)$(CP) $(SRCS) $(STAGEDIR)/source
	$(AT)$(CP) -r $(STAGEFILES) $(STAGEDIR)
	$(ECHO) "[$(FG_GREEN)ZIP$(NORMAL)] $(STAGEFILES) > $(ZIPOUT)"
	$(AT)$(ZIP) -qr9 $(ZIPOUT) $(STAGEDIR)

prep:
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FMC16LX/m_flash.* kernal16/
	$(AT)$(CP) /c/Program\ Files\ \(x86\)/FUJITSU/FUJITSU\ FLASH\ MCU\ Programmer/FMC16LX/CHIPDEF.INI chipdef16.ini

help:
	$(AT)$(CAT) readme.txt

