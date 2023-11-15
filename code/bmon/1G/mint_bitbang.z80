; *************************************************************************
;
;       MINT Minimal Interpreter for the Z80 
;
;       Ken Boak, John Hardy and Craig Jones. 
;
;       GNU GENERAL PUBLIC LICENSE                   Version 3, 29 June 2007
;
;       see the LICENSE file in this repo for more information 
;
; *****************************************************************************

;bit bang serial routines

        ; bit bang baud rate constants @ 4MHz
B300:	.EQU	0220H
B1200:	.EQU	0080H
B2400:	.EQU	003FH
B4800:	.EQU	001BH
B9600:	.EQU	000BH

;initialise the bit bang serial port
;-----------------------------------
InitialiseSerial:
        LD    HL,$2000          ;power up delay
        CALL  bitime
        LD    A,$40
        LD    C,SCAN
        OUT   (C),A             ;make the output port high
        LD    HL,B4800
        LD    (BAUD),HL         ;set up the baud rate
        RET

; bit bang serial transmit routine
;---------------------------------
; transmit a byte via an output port pin

; entry : A = byte to transmit
;  exit : no registers modified

TxChar:
    	PUSH	AF
    	PUSH	BC
    	PUSH	HL
    	LD	HL,(BAUD)   ;HL holds the bit time delay
    	LD	C,A

;transmit the start bit
	XOR	A
	OUT	(SCAN),A
	CALL	bitime

; transmit 8 bits of data
	LD	B,08H
	RRC	C
nxtbit:	
    RRC	C	            ;shift bits to D6 
	LD	A,C	            ;send lsb first
	AND	$40	            ;for one bit time
	OUT	(SCAN),A
	CALL	bitime
	DJNZ	nxtbit

;send the stop bits
    LD	A,40H
    OUT	(SCAN),A
    CALL  bitime
    CALL  bitime
	POP	HL
	POP	BC
	POP	AF
	RET

; bit bang serial receive routine
;--------------------------------
;receive a byte via an input port pin

; entry : none
; exit : A = received byte if carry clear

; AF registers modified

RxChar:
	PUSH	BC
	PUSH	HL
 
;wait for the start bit high to low transition
startbit:
        IN	A,(KEYBUF)
	    BIT	7,A
	    JR	NZ,startbit	;no start bit yet

; start bit detected
	LD	HL,(BAUD)       ;delay for half a
	SRL	H               ;bit time to sample the middle
	RR	L 	            ;of the bit cell
	CALL 	bitime
	IN	A,(KEYBUF)
	BIT	7,A             ;check the start bit is still low
	JR	NZ,startbit 

; valid start bit detected
	LD	B,$08
getbits:	
    LD	HL,(BAUD)
	CALL	bitime      ;delay one full bit time
	IN	A,(KEYBUF)
	RL	A
	RR	C               ;shift bit into output reg
	DJNZ	getbits
	LD	A,C
	OR	A               ;clear carry flag
    POP	HL
    POP	BC
	RET

; bit time delay
;---------------
;delay for one serial bit time
;entry : HL = delay time
;no registers modified

bitime:
        PUSH  HL
        PUSH  DE
        LD    DE,0001H
bitim1:  
        SBC   HL,DE
        JP    NC,bitim1
        POP   DE
        POP   HL
        RET
