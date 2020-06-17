# TEC Games

## General Setup
To be run on the TEC-1D with any Monitor.  For the keyboard to work it requires EITHER a 4k7 (or 2k2 or lower) resistor between the NMI (pin 17 on Z-80) and D6 (pin 10 on the Z-80) OR the DAT (LCD) expanstion board fitted to port 3.  The current TEC-1D boards have the JMON MOD resitor connection already there.

The 8x8 LED board is fitted to ports 5 and 6 with the port select strobe of the left hand latch going to port 6.

All ROM files are to be loaded at address **0x0900**.  If this doesn't suit then just complie the **.Z80** file and change the starting **.ORG** address.  They can be manually typed using the **.lst** file but I recommend using a serial loader.  See my  [SIO Transfer](https://github.com/bchiha/TEC-1D-Stuff/tree/master/sio_transfer) project.

## TEC Invaders (The Return)
This is my version of the TEC invaders that was originally writen by __Cameron Sheppard__

### To Play
Use '1' to move Left, '9' to move right and '+' to shoot.  10 Points for hitting an invader and 50 Points for the UFO on the top row.  Player has 3 lives and the game is over when all lives are lost.  You lose a life if the invaders land or you get hit by one of their bombs.  Invaders come in waves and get faster with less invaders alive.

## TEC Maze Treasure Hunt
### To Play
The aim is to move around the maze, and pick up all glowing orbs. Once all are found, get back to your starting spot.  To move use the following keys

         6
     1 < X > 9
         4

Your character is a 1x1 Steady LED.  Orbs are flashing (glistening!) 1x1 LED's.  The Seven Segments display your current location (Address) and the number of treasures to collect (Data).  You must find all orbs to win.  Just move over the orb to pick it up but only when you can see it!

The Maze has a total of 64 rooms on an 8x8 grid.  Rooms are represented vertically with A-H and horizontally with 0-7.  Top left is 'A0'.  Orbs and initially player positions are randomly placed at the start of the game.  The number of orbs to find is random for each game.