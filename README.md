# How to get this running with a cooqrobot/elechouse pn532 breakout board:

1. carefully unsolder the 0 ohm resistor jumper from the HIS0 connector, then resolder it (or just bridge the pads with solder) so that the '0' side is connected rather than the 1, this puts the reader in HSU mode.

2. Solder headers onto the 8 pin connector on the board where the tx/rx pins are, you need at least tx,rx,3.3v,gnd. N.B. you can't use the SCL/SDA pins on the 4 pin connector cos they have pullups on them...

2. Connect 3.3v and gnd to somewhere convient on the connected launchpad

4. This sketch uses Serial6 which uses PP0 and PP1, aka pins 43 and 44 on the connected launchpad, connect those too, PP0 goes to RX on the board and PP1 to TX.

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
* Out of service, maintainer: yellow (and the tool will turn off)

# To use the button menu:

A short press starts the menu and switches between items, a long press selects a menu item.

For users the only menu item is to take the tool out of service - it's flashing red/green

## For maintainers the menu items are:

* Add a user (flashing Amber)
* Take the tool out of service (if it's in service) - flashing red
* put a tool in service (if it's out of service), flashing green.

## So for a user to take a tool out of service:

Press the button once, quickly, the button will be flashing red/green.
Hold the button down for a second or so and release.
The button will go solid red and the tool will be out of service

## For a maintainer to access the menu and do nothing:

press the button once, the button will flash Amber/yellow - the acnode is in add mode
press the button again, the button will flash red/yellow - the menu is in take tool out of service mode.
press the button again, the button will stop flashing, you have exited the menu

## For a maintainer to add a card:

Press the button, it will flash Amber/yellow
press the button for 1 sec or so and let it go, the button will go yellow
remove the maintainer card. add the card to be added - the button will go orange (invalid card colour)
remove the card.
The new card should now be added.

The menu also times out after 60 seconds.

