# BMON - The Big (Brian's) Monitor
## The essential companion for the [TEC-1F](https://github.com/crsjones/TEC-1) Z80 Single Board Computer
---

BMON is a full 8k Monitor for the TEC-1F.  It is designed for the modern Z80 coding enthusiest.  The user develops/debugs Z80 code on their own computer and then when ready, send the program to the TEC-1F via the Serial Port.

At the core of BMON, is all the good bits of [JMON](https://github.com/tec1group/Software/monitors/JMon/) (Jim's Monitor), but without the TAPE transfer routines.  In place of this are Serial Bit-Bang routines to receive and send data via a USB to Serial Adaptor.

## Memory Map
BMON is to be inserted in the ROM socket on the TEC-1F.  The initial user starting address is `0x2000`
| **Address** | **Description** |
| ------- | ----------- |
| 0x0000 - 0x1FFF | BMON Monitor Read only |
| 0x2000 - 0x3EFF | User RAM |
| 0x3F00 - 0x3FFF | BMON Reserved RAM |
| 0x4000 - 0x7FFF | RAM Expansion Port (Add-On Slot) |
| 0x8000 - 0xFFFF | User Access via Memory Headers |

Between `0x3F00 - 0x3FFF` is a reserved area for BMON.  Some parts of this area can be modified by the user (see below), but in general it is advised not to modify.  The Stack is also in this area starting at `0x3F20`.

## JMON Features within BMON
There are some really nice features that JMON has which have been incorporated into BMON.  Some of these are not well known.  The following features on JMON has been retained on BMON.
- **KeyPad:** All keys have an 'auto repeat' behaviour.  Simply, hold the key down for 1 second and the key press will repeat.  This is useful when using the `+` and `-` keys for address moving.  When data is being entered, the address will auto increment to the next address when a byte has been entered.  If a mistake is made, just press `AD` twice to re-key in the data.
- **RST xx:** Z80 Restart jump blocks can be used to assist with keyboard/screen features
    - `RST 08H` This simulates a `HALT` instruction by checking that a key is released, and looping until a key is pressed.  Register `A` and `I` will store the key pressed.
    - `RST 18H` This does an seven segment "scan" and check if a key has been pressed.  It requires continual looping.  If the `ZERO` flag is set, or Register `A` doesn't contain `0xFF`, a key has been pressed.  The data that is displayed on the seven segments at the location of the address in `V_DISPLAY` (see below).
    - `RST 28H` This starts the Single Line Stepper from the current location in the code.  An Interrupt Adaptor is needed for this feature to work (see below).
    - `RST 30H` This checks the busy state of the LCD Add-on.  The RST call will exit when the LCD is not busy.  Useful when sending data to the an LCD Add-on.
    - `RST 38H` This calls the Stepper routine when a hardware Interrupt is triggered.  A Hardware Adaptor is needed as the TEC-1 doesn't have anything connected to the `INT` line (see below).

## External Jump Table
The following routines can be used in external software.  To access them use `CALL xxxx`, where xxxx is the address of the routine in the jump block.
| Address | Routine |
| ------- | ------- |
| 0x0041 | Menu Routine |
| 0x0044 | Perimeter Handler Routine |
| 0x0047 | Menu Routine (Soft entry) |
| 0x004A | Bit Bang Serial Transit Byte Routine |
| 0x004D | Bit Bang Serial Receive Byte Routine |
| 0x0050 | Perimeter Handler (Soft entry) |
| 0x0053 | Bit Bang Delay Routine |
| 0x0056 | Ascii Scroller Routine |
| 0x0059 | Music Routine |
### Menu Routine
`CALL 0x0041`

## Single Stepper

## Soft / Hard Reset

## Shift Key 
The Shift key has an important feature that when used in conjunction with other keys calls special routines.  The most important one is `Shift-0`.  This calls the Main Menu routine.  Here is what each Shift-n key does.
| Key | Feature | Key | Feature | Key | Feature |
| :---: | ------- | :---: | ------- | :---: | ------- |
| 0 | Main Menu | 1 | Address - 1 | 2 | Call Stepper |
| 4 | Address + 4 | 5 | Soft Reset | 6 | Address - 4 |
| 7 | Show Registers | 8 | Address + 8 | 9 | Address + 1 |
| A | Address - 8 | | | |

Using the Address jump routines, can quickly get you to the address that you need.  They are placed in a Plus formation on the keypad   

## Main Menu



