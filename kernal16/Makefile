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

# Master Makefile to assemble flash loader kernals.

#To get a fully verbose make output do declare 'AT' at the command line like so: 'make AT='.
#By default AT is undefined and thus assigned the string '@'.
AT ?= @

ECHO=@echo -e

# Attach debugging symbols and activate debug printout.
DEBUGGING	= false

FETOOL=/c/Softune
FELANG=ASCII
TMP=/c/Users/kari/AppData/Local/Temp
INC907=/c/Softune/lib/907/include
OPT907=/c/Softune/lib/907

# Assembler executable.
ASM = $(FETOOL)/bin/FASM907S.EXE

ASMFLAGS = -cpu MB90F598G

# Any platform neutral options to the linker.
#LDFLAGS	=

#RULES		+= <rules_list>		# List of make rules to run but after the default.

#AFTER_INSTALL	+= <shell commands> # Command line to execute as the last step in the install directive.

# List of source files to compile.
SRCS = k_flash_595.asm

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
endif

# Select debugging or optimized compiler flags.
ifeq ($(DEBUGGING), true)
ASMFLAGS += -g
endif

CLEANFILES += $(OBJS)

### Rules
.SUFFIXES : .asm .obj
.PHONY: clean mrproper

RULES += $(OBJS) $(OUTPUT)$(EXT)

OBJS = $(SRCS:.asm=.obj)

all: $(RULES)

.asm.obj:
	$(ECHO) "[$(FG_CYAN)assemble$(NORMAL)] $@"
	$(AT)$(ASM) $(ASMFLAGS) -o $*.o $<
#	$(ECHO) "[$(FG_GREEN)LINKING$(NORMAL)] $(OUTPUT)$(EXT)"
#	$(AT)$(LD) $(LDFLAGS) -c -o $*.o $<

clean:
	$(ECHO) "[$(FG_DKYELLOW)CLEAN$(NORMAL)] $(CLEANFILES)"
	$(AT)$(RM) -rf $(CLEANFILES)

