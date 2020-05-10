My TEC to Z80 SIO adaptor

I acidently purchases some Z80 SIO (Serial IO) chips and figured out how to connect it to the TEC so I can transfer data to and from my computer.

- Using a clock speed of 3.6864 MHz which divides by 64 to get a whole number baud of 57600.
- Using IO Port 7 for Chip Enabled and A4,A5 for Data/Control and A/B SIO ports.  Using A4-5 doesn't mess with the TEC A0-2 and I can do something similar to the LCD output for control and data.  IE: OUT(27),a and OUT(07),a.
- Using a USB to FTDI adaptor ($2.28 from AliExpress).
- Added some D-Flip Flops and jumpers to divide the clock to 28800 and 14400 baud. (Acually haven't tested this yet).
- Using Coolterm as my serial terminal which can send and capture files.
- Didn't use IM 2 but could have.  I only run the code when I need to transfer, Its not like the TEC is doing something else during transfer.
- I use JMON's Menu to select the transfer type and the Perimeter to select start address and length.  If not using JMON then code can be called directly.
- All pins needed are through headers on the TEC-1D except IORQ which I wire from under the board.
