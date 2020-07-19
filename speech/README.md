
# TEC Speech Module

Code and design for the TEC Speech board that uses the SP0256A-AL2 chip.  This chip is hard to find now and there are many fakes out there so ensure you have an original.  Some fakes are actually the SP0256-012 (Mattel Chip) and some crash the system.  I've include a picture of the original chip.  You can get them from http://www.sciencestore.co.uk/acatalog/Electronics.html


## Text 2 Allophone converter - text2allophone.c
Compile and run, then type some text.  Type **PP** to insert a 200ms pause and **EOF** to insert `0xFF` (End of file for TEC running program).  Control-D to exit.
**Note:** Puctuation must be seperated with a space or don't use it at all.

Output is given as Allophones and the SP0256a-AL2 hex equivilant.  If a word can't be found "**??**" is outputed and a "**.**" represents a word gap.  A pause of 100ms is automatically placed between words.

To be used with the TEC Speech Module attached to **Port 7**.

**Options**
|  |  |
|--|--|
|**-b**| Output Binary File which includes main program code |
| **-w** | Don't include main program in bianary file (used with -b) |

For binary output to directly load into the TEC. use -b as a command line option.  Binary files are created with the code to activate the Speech Module and the speech data.  If you just want the allophones to be outputted use -w as well.  A new file is created after each Carriage Return.  Binary file are to be loaded at address `0x900` on the TEC

## Files that are requried are:

| | |
|-|-|
|text2allophone.c|This Program|
|cmudict-0.7b|CMU dictionary file that associates words to the ARPAbet phoneme set.  See http://www.speech.cs.cmu.edu/cgi-bin/cmudict|
|cmu2sp0.symbols|Convert CMU to SPO256a Allophones|

## Sample output from the program

    > Welcome to Talking Electronics PP I hope you have a great day ! EOF
    TXT> WW EH LL KK2 AX MM . TT2 UW2 . TT2 AO KK2 IH NG . IH LL EH KK2 TT2 RR1 AA
         NN1 IH KK2 SS . AY . HH2 OW PP . YY2 UW2 . HH2 AE VV . AX . GG3 RR1 EY TT2 .
         DD2 EY . EH KK2 SS KK2 LL AX MM EY SH AX NN1 PP OY NN1 TT2 .

    000> 2E 07 2D 29 0F 10 03 0D
    008> 1F 03 0D 17 29 0C 2C 03
    010> 0C 2D 07 29 0D 0E 18 0B
    018> 0C 29 37 03 04 06 03 39
    020> 35 09 03 19 1F 03 39 1A
    028> 23 03 0F 03 22 0E 14 0D
    030> 03 21 14 03 07 29 37 29
    038> 2D 0F 10 14 25 0F 0B 09
    040> 05 0B 0D 03 FF

For reference, here is the Test Program that is used to run the Speech Module taking in data for the SP0256a-AL2 chip.  It is included in the binary output by default

    0900	21 10 09	LD HL,0910	;Location of allophone data
    0903	7E		LD A,(HL)	;Load A with the next allophone
    0904	FE FF 		CP 0xFF		;Compare A with 0xFF (EOF for data)
    0906	28 05       	JR Z,090D       ;IF EOF then jump to address 090D
    0908    D3 07		OUT (07),A      ;Output A to port 7 on the TEC
    090A	23		INC HL          ;Index to the next allophone
    090B	18 F6       	JR 0903         ;Jump back to 0903 to say the next allophone
    090D    76          	HALT            ;Wait for key input as EOF has reached
    090E    18 F0       	JR 0900         ;Jump back to start

