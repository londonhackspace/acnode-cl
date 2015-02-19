
# Parts list

## For the transistor switch

* 1 x NPN transistor, i used a 2N3904
* 1 x 1K resistor
* 1 x small bit (8 x 8 holes is fine) of vero board
* 2 x C shaped crimp or solderable terminals for the relay
* 2 x wires with female header socket ends (the other end dosn't matter, and will be cut off), one black, and one red.
* 1 x black female to female header socket lead
* 1 x grey female to female header socket lead

## For the rest of the node:

* 2 x crimp or solderable spade terminals for the microswitch on the arcade button
* 2 x wires with female header socket ends (the other end dosn't matter, and will be cut off), one black & one white
* 1 x PN532 based card reader
* 1 x connected launpad
* 1 x red  female to female header socket lead
* 1 x blue female to female header socket lead
* 1 x green female to female header socket lead
* 1 x black female to female header socket lead

##  Making the transistor switch

* cut one end off the 2 wires with header socket ends, leaving as much wire
as possible and the female ends

* Strip the bare end crimp into the terminals - for the arcade button
terminals use a white and black wire, and for the relay use red and black
wires.

* Attach the red wire to the+ side of the VDC connections on the SSR and the
black one to the - side.

* Attach the red female and to +5v heder pin on the launchpad

* On some strip board solder the transistor on one edge with the flat side
facing the rest of the board.

* Cut the track connected to the middle (base) pin of the transistor in the
middle of the board

* Solder the 1K resistor over the hole you've cut

* Solder a 3 pin header so the middle pin goes header -> 1k resistor ->
middle (base) of the transistor, and the left and right pins are connected
to the left and right pin of the transistor

pic of stripboard + transistor + header:

https://pointless.net/photos/id/1419346624/

pic of the bottom showing cut track:

https://pointless.net/photos/id/1419346632/

* with the header pins towards you looking edge on at the board (the flat
side of the transistor should be facing you), the pins are, from left to
right:

* 1 connect to gnd on the connected launchpad with a black female to female lead.
* 2 connect to PG_1 on the connected launchpad
* 3 connect to - on the ssr VDC side with the black lead you made eairler

## Wiring in the arcade switch

* make two spade terminal leads, one white and one black

like this:

https://pointless.net/photos/id/1419334749/

* connect the black one to the bottom of the microswitch on the button and the other end to a convient GND on the launchpad

* connect the white end to the terminal labeled "NO" on the microswitch, and the other end to PF_1 on the launchpad

* connect the white lead of the rgb led leads to +5V on the launchpad

* the RGB red, green and blue leads go to PM0, PM1 and PM2

## How to get this running with a cooqrobot/elechouse pn532 breakout board:

1. carefully unsolder the 0 ohm resistor jumper from the HIS0 connector, then resolder it (or just bridge the pads with solder) so that the '0' side is connected rather than the 1, this puts the reader in HSU mode.

2. Solder headers onto the 8 pin connector on the board where the tx/rx pins are, you need at least tx,rx,3.3v,gnd. N.B. you can't use the SCL/SDA pins on the 4 pin connector cos they have pullups on them...

2. Connect 3.3v and gnd to somewhere convient on the connected launchpad

4. This sketch uses Serial6 which uses PP0 and PP1, aka pins 43 and 44 on the connected launchpad, connect those too, PP0 goes to TX on the board and PP1 to RX.

5. Check the labels on the card reader boards carefully, one cheap reader board has the pins for the ftdi cable pinouts *backwards* on the top (component) side of the board. Also some card readers label TX and RX the other way round...

See here for the Connected Launchpad pinouts:

http://energia.nu/pin-maps/guide_tm4c129launchpad/

N.B.: use energia-0101E0012, not energia-0101E0013, as 13 breaks things (i think it defines a function called wakeup which clashes with the one in the PN532 stuff).

# You will need 2 libraries:

## Syslog from here:

https://github.com/londonhackspace/Syslog

put it in the libraries folder under your energia sketches folder

## and the PN532 libraries from here:

https://github.com/londonhackspace/PN532

You'll need to copy or symlink PN532 and PN532_HSU into your energia-sketches libraries directory as well.

# using the CLI

Once the firmware has been uploaded to the connected launchpad connect with a serial terminal (if you use a real one rather than the Energia serial monitor you get command history and line editing).

Use `help` to see a brief description of commands. You want to set the nodeid and toolname as the minimum:

```
nodeid <id>
name <name>
server <acserver hostname>
port <http port to use>
syslog <syslog server to use>
```

and then `save` to save the settings.

# Button status/colours:

* Starting up: yellow
* In service, idle: blue
* In service, valid user, tool on: green
* In service, invalid user: orange
* In service, maintainer: yellow
* Out of service, idle: flashing blue/red
* Out of service, valid user: red (and the tool will be off)
* Out of service, maintainer: yellow (and the tool will turn on)

# To use the button menu:

A short press starts the menu and switches between items, a long press selects a menu item.

For users the only menu item is to take the tool out of service - it's flashing red/green

The menu also times out after 60 seconds.

## For maintainers the menu items are:

* Add a user (flashing Amber)
* Take the tool out of service (if it's in service) - flashing red
* put a tool in service (if it's out of service), flashing green.

## So for a user to take a tool out of service:

Press the button once, quickly, the button will be flashing red/green.

Hold the button down for a second or so and release.

The button will go solid red and the tool will be out of service

## For a maintainer to access the menu and do nothing:

press the button once, the button will flash Amber/yellow - the acnode is in add mode.

press the button again, the button will flash red/yellow - the menu is in take tool out of service mode.

press the button again, the button will stop flashing, you have exited the menu.

## For a maintainer to add a card:

Press the button, it will flash Amber/yellow

press the button for 1 sec or so and let it go, the button will go yellow

remove the maintainer card. add the card to be added - the button will go orange (invalid card colour)

remove the card.

The new card should now be added.
