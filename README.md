How to get this running with a cooqrobot/elechouse pn532 breakout board:

1. carefully unsolder the 0 ohm resistor jumper from the HIS0 connector, then resolder it (or just bridge the pads with solder) so that the '0' side is connected rather than the 1, this puts the reader in HSU mode.

2. Solder headers onto 8 pin connector the board where the tx/rx pins are, you need at least tx,rx,3.3v,gnd.

2. Connect 3.3v and gnd to somewhere convient on the connected launchpad

4. This sketch uses Serial6 which uses PP0 and PP1, aka pins 43 and 44 on the connected launchpad, connect those too, PP0 goes to RX on the board and PP1 to TX.

See here for the Connected Launchpad pinouts:

http://energia.nu/pin-maps/guide_tm4c129launchpad/

N.B.: use energia-0101E0012, not energia-0101E0013, as 13 breaks things (i think it defines a function called wakeup which clashes with the one in the PN532 stuff).

You will need 2 libraries:

syslog from here:

https://github.com/londonhackspace/Syslog

put it in the libraries folder under your energia sketches folder

and the PN532 libraries from here:

https://github.com/londonhackspace/PN532

You'll need to copy or symlink PN532 and PN532_HSU into your energia-sketches libraries directory as well.

Once the firmware has been uploaded to the connected launchpad connect with a serial terminal (if you use a real one rather than the Energia serial monitor you get command history and line editing).

Use `help` to see a brief description of commands. You want to set the nodeid and toolname as the minimum:

```
nodeid <id>
name <name>
```

and then `save` to save the settings.
