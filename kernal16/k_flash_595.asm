;Kuji16 Flash MCU Programmer
;Copyright (C) 2014-2016 Kari Sigurjonsson

;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.

;You should have received a copy of the GNU General Public License
;along with this program.  If not, see <http://www.gnu.org/licenses/>.

          .PROGRAM  STARTUP
          .TITLE    "Pump it up"

          .SECTION  VERSIONS, CONST
          .SDATA    "Start 1.12\n"      ; comment this line to remove

#set      OFF       0
#set      ON        1
#set      MB90500   3
#set      FAMILY    MB90500        ; <<< select family
#set      SMALL     0              ;     16 Bit    16 Bit
#set      MEMMODEL  SMALL          ; <<< C-memory model

#set      ROMCONST  0              ; works only with compiler ROMCONST
#set      RAMCONST  1              ; works with BOTH compiler settings

#set      CONSTDATA RAMCONST       ; <<< set RAMCONST or ROMCONST

#set      USRSTACK  0              ; use user stack, system stack for interrupts
#set      SYSSTACK  1              ; use system stack for all (program + inter)

#set      STACKUSE  SYSSTACK       ; <<< set used stacks

SSSIZE         .EQU 1024           ; <<< system stack size in words
USSIZE         .EQU 1              ; just a dummy

#  macro RELOAD_SP                 ; used after function call
          MOVW A, #SSTACK_TOP      ; repair stack, in case stream_init
          MOVW SP,A                ; was completed by RET (not RETP)
#  endm

#set      REGBANK   0              ; <<< set default register bank

#set          NOLIBINIT 0          ; do not initialize Library
#set          DOLIBINIT 1          ; initialize Library interface

#set          CLIBINIT  OFF        ; <<< select extended libray usage

#set      NOCLOCK        0         ; do not touch CKSCR register
#set      MAINCLOCK      1         ; select main clock (1/2 external)
#set      PLLx1          2         ; set PLL to x1 ext. clock/quartz
#set      PLLx2          3         ; set PLL to x2 ext. clock/quartz
#set      PLLx3          4         ; set PLL to x3 ext. clock/quartz
#set      PLLx4          5         ; set PLL to x4 ext. clock/quartz

#set      CLOCKSPEED    NOCLOCK    ; <<< set PLL ratio
#set      CLOCKWAIT     ON		   ; <<< wait for stabilized PLL, if
                                   ;     PLL is used

#set      SINGLE_CHIP    0         ; all internal
#set      INTROM_EXTBUS  1         ; mask ROM, FLASH, or OTP ROM used
#set      EXTROM_EXTBUS  2         ; full external bus (INROM not used)

#set      BUSMODE SINGLE_CHIP      ; <<< set bus mode (see mode pins)

#set      ROMMIRROR      ON        ; <<< ROM mirror function ON/OFF
                                   ;     MB90500/400 family only

#set      WIDTH_8        0         ; 8 Bit external bus
#set      WIDTH_16       1         ; 16 Bit external bus

#set      AUTOWAIT_IO    0         ; <<< 0..3 waitstates for IO area
#set      AUTOWAIT_LO    0         ; <<< 0..3 for lower external area
#set      AUTOWAIT_HI    0         ; <<< 0..3 for higher external area

#set      ADDR_PINS B'00000000     ; <<< select used address lines
                                   ;     A23..A16 to be output.

#set      BUS_SIGNAL B'00000100    ; <<< enable bus control signals
;                      ||||||||__ ignored
;                      |||||||___ bus width lower memory (0:16, 1:8Bit)
;                      ||||||____ output WR signal(s)    (1: enabled  )
;                      |||||_____ bus width upper memory (0:16, 1:8Bit)
;                      ||||______ bus width ext IO area  (0:16, 1:8Bit)
;                      |||_______ enable HRQ input       (1: enabled  )
;                      ||________ enable RDY input       (1: enabled  )
;                      |_________ output CLK signal      (1:enabled   )

#set      iARSR     ((AUTOWAIT_IO<<6)|((AUTOWAIT_HI&3)<<4)|(AUTOWAIT_LO&3))

#set      RESET_VECTOR   OFF       ; <<< enable reset vector

#  set    MODEBYTE       0

LPMCR     .EQU      0xA0           ; Low power mode control register
CKSCR     .EQU      0xA1           ; Clock select control register
ROMM      .EQU      0x6F           ; ROM mirror control register
WDTC      .EQU      0xA8           ; Watchdog control register
TBTC      .EQU      0xA9           ; Timerbase timer control register

          .SECTION  DATA,      DATA,   ALIGN=2  ; zero clear area
          .SECTION  INIT,      DATA,   ALIGN=2  ; initialized area
          .SECTION  DIRDATA,   DIR,    ALIGN=2  ; zero clear direct
DIRDATA_S:
          .SECTION  DIRINIT,   DIR,    ALIGN=2  ; initialized dir

          .SECTION  CINIT,     DATA,   ALIGN=2  ; initialized const

          .SECTION  LIBDATA,   DATA,   ALIGN=2  ; zero clear lib area
          .SECTION  LIBINIT,   DATA,   ALIGN=2  ; initialized lib area

          .SECTION  DIRCONST, DIRCONST,ALIGN=2  ; DIRINIT initializers
          .SECTION  DCONST,    CONST,  ALIGN=2  ; DINIT initializers
          .SECTION  CONST,     CONST,  ALIGN=2  ; CINIT initializers
          .SECTION  LIBDCONST, CONST,  ALIGN=2  ; LIBDCONST init val

          .SECTION  DCLEAR,    CONST,  ALIGN=2  ; far zero clear table
          .SECTION  DTRANS,    CONST,  ALIGN=2  ; far copy table

               .SECTION  SSTACK, STACK, ALIGN=2
          .RES.H    SSSIZE
SSTACK_TOP:
               .SECTION  USTACK, STACK, ALIGN=2
          .RES.H    USSIZE
USTACK_TOP:

          .SECTION  CODE_START, CODE, ALIGN=1
          .IMPORT   _main                    ; user code entrance
          .EXPORT   _start

notresetyet:
          NOP

_start:
          AND  CCR, #0             ; disable interrups
          MOV  ILM,#7              ; set interrupt level mask to ALL
          MOV  RP,#REGBANK         ; set register bank pointer

          MOV  I:ROMM, #ROMMIRROR

#macro    SEGCOPY DEST, SRC
          MOV  A,#BNKSEC \SRC      ; get bank of source section
          MOV  DTB,A               ; store source bank in DTB
          MOV  A,#BNKSEC \DEST     ; get destination bank
          MOV  ADB,A               ; store dest bank in ADB
          MOVW RW0,#SIZEOF (\DEST) ; get size of dest section
          MOVW A,#\DEST            ; move destination offset to AL
          MOVW A,#\SRC             ; move source offset to AL and
                                   ; move AL (dest offset) to AH
          MOVSI     ADB,DTB        ; copy RW0 bytes src->dest
#endm

          SEGCOPY   INIT, DCONST        ; from DCONST to INIT
          SEGCOPY   DIRINIT, DIRCONST   ; from DIRCONST to DIRINIT
          SEGCOPY   LIBINIT, LIBDCONST  ; from LIBDCONST to LIBINIT
          SEGCOPY   CINIT, CONST        ; from CONST to CINIT

#macro    SEGZERO SEC
          MOV  A,#BNKSEC \SEC      ; get bank of section
          MOV  ADB,A               ; store bank in ADB
          MOVW RW0,#SIZEOF (\SEC)  ; store number of bytes in RW0
          MOVW A,#\SEC             ; move dest offset to AL
          MOVN A,#0                ; move fill value to AL and
                                   ; move AL (offset) to AH
          FILSI     ADB            ; fill RW0 bytes with AL
#endm

          SEGZERO   DATA           ; clear DATA
          SEGZERO   DIRDATA        ; clear DIRDATA
          SEGZERO   LIBDATA        ; clear LIBDATA

          MOV  A, #BNKSEC DTRANS   ; get bank of table
          MOV  DTB, A              ; store bank in DTB
          MOVW RW1, #DTRANS        ; get start offset of table
          OR   CCR, #H'20          ; System stack flag set (SSB used)
          BRA  LABEL2              ; branch to loop condition
LABEL1:
          MOVW A, @RW1+6           ; get bank of destination
          MOV  SSB, A              ; save dest bank in SSB
          MOVW A, @RW1+2           ; get source bank
          MOV  ADB, A              ; save source bank in ADB
          MOVW A, @RW1+4           ; move destination addr in AL
          MOVW A, @RW1             ; AL ->AH, src addr -> AL
          MOVW RW0, @RW1+8         ; number of bytes to copy -> RW0
          MOVSI     SPB, ADB       ; copy data
          MOVN A, #10              ; length of one table entry is 10
          ADDW RW1, A              ; set pointer to next table entry
LABEL2:
          MOVW A, RW1              ; get address of next block
          SUBW A, #DTRANS          ; sub address of first block
          CMPW A, #SIZEOF (DTRANS) ; all blocks processed ?
          BNE  LABEL1              ; if not, branch

          MOV  A, #BNKSEC DCLEAR   ; get bank of table
          MOV  DTB, A              ; store bank in DTB
          MOVW RW1, #DCLEAR        ; get start offset of table
          BRA  LABEL4              ; branch to loop condition
LABEL3:
          MOV  A, @RW1+2           ; get section bank
          MOV  ADB, A              ; save section bank in ADB
          MOVW RW0, @RW1+4         ; number of bytes to copy -> RW0
          MOVW A, @RW1             ; move section addr in AL
          MOVN A, #0               ; AL ->AH, init value -> AL
          FILSI     ADB            ; write 0 to section
          MOVN A, #6               ; length of one table entry is 6
          ADDW RW1, A              ; set pointer to next table entry
LABEL4:
          MOVW A, RW1              ; get address of next block
          SUBW A, #DCLEAR          ; sub address of first block
          CMPW A, #SIZEOF (DCLEAR) ; all blocks processed ?
          BNE  LABEL3              ; if not, branch

#macro SYSSTACKINI
          OR   CCR,#H'20               ; set System stack flag
          MOV  A,#BNKSYM SSTACK_TOP    ; System stack set
          MOV  SSB,A
          MOVW A,#SSTACK_TOP
          MOVW SP,A
#endm
#macro USRSTACKINI
          AND  CCR,#H'DF               ; User stack flag set
          MOV  A,#BNKSYM USTACK_TOP    ; User stack set
          MOV  USB,A
          MOVW A,#USTACK_TOP
          MOVW SP,A
#endm
          USRSTACKINI
          SYSSTACKINI                  ; finally system stack selected

          MOV  A,#BNKSEC DATA          ; User data bank offset
          MOV  DTB,A

          MOV  A,#PAGE DIRDATA_S       ; User direct page
          MOV  DPR,A

          CALL _main               ; Start main function

end:
          BRA  end                 ; Loop

          .END notresetyet         ; define debugger start address

