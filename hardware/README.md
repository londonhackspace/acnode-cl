# How to make an ACnode or Doorbot

Over the years the design of the ACnodes and doorbots have been gradually improved.

As of 2021-10-28, all acnodes and doorbots in London Hackspace have been using the 'New-ACNode-Doorbot-Shield' designs. This is a single PCB designed to
fit **underneath** a TI Tiva C XL Connected Launchpad microcrontroller board EK-TM4C1294XL EVAL BRD, TIVA C CONNECTED LAUNCHPAD. The Launchpad boards are normally available from Farnell and RS.

There are plans for an all-new doorbot/acnode board together with a reader module, but they are not as yet ready for use. This is to address interference on long cable runs between the acnode/doorbot board and reader.

The board features an on-board 5V coil relay that is suitable for switching up to 10A of 230V. If you need to switch anything greater, then use it to
energise the coil of an appropriate contactor.

Build up the ACnode board as per kicad schematics. On rev 1.1b, component values are silkscreened for easy assembly. You will require long header pins so that the Launchpad board sits above the relay. Hint when soldering the long header pins into this board - use a Launchpad upside-down as a jig to hold the pins, place the pins, place the ACnode board upside-down, then solder them. Once soldered, a blob of hot glue is advised to keep the long header pins spaced and prevent bending them when handling in future.
When building the board for the ACnode role, you may omit the LM7805 voltage regulator if you're only going to be providing it with +5V power.

Always ensure that any metal enclosure is suitably earthed.

## Doorbot role

You will require

- A +12V power supply, approximately 3A at 12V should be enough to energise a maglock or solenoid lock. You can't use POE for this, it won't provide enough current for a maglock.
- Optional: A sealed lead acid battery, recommended for exterior doors. The doorbot software is designed to handle power outages with no networking available.
- A suitable helpfully labelled DOOR EXIT button, SPDT or DPDT, must be able to handle the current of whatever lock you choose. We will use two poles to both interrupt the current to the lock (NC contact) and to signal the doorbot to hold the door open for a time.
- Some CAT5 or higher wiring and connectors to run a cable to the reader module.
- Some suitable wire. We've found that old +12V DC power cables for CCTV cameras is suitable for the 12V lock circuit.
- A suitable enclosure
- A micro-USB to laptop cable, of suitable length, for configuration and flashing the software.
- A maglock with bracket, or solenoid deadbolt. 12V DC.

## ACnode role

You will require

- A +5V power supply, most decent USB power supplies work well, and you can get POE splitters that will provide this via Micro USB as well. Most of the London Hackspace network switches support POE. This is normally the recommended solution.
- Some CAT5 or higher wiring and connectors to run a cable to the reader module.
- Knowledge of the tool that you are providing access control to. Ideally, you'd be switching a low(er) voltage control circuit like an e-stop, rather than turning full power on and off to the tool. For high current switching, use an external suitably sized mains contactor.
- optional: a tool-running input, like on the laser cutter, the AC node can log the time the tool was running.
- suitable enclosure, or suitable space within the tool for the board. DIN rail mounting is recommended, 3D printed DIN-rail clips have been known to work with the board glued to the clips. Where we need a standalone enclosure, we have used TA 241911 ENCLOSURE BOX, GREY, IP65, 240X191X107MM, Farnell Part number 1422671
- A micro-USB to laptop cable, of suitable length, for configuration and flashing the software.

Set the power select jumper on the Launchpad board to 'OTG', and plug your USB power supply into the 'OTG' interface, we'll use that to provide power to the microcontroller.

## Reader enclosure

Typically used for ACnode roles.
Can use any suitable enclosure, but ideally one deep enough for the arcade button, and a RJ45 modular connector. https://uk.farnell.com/schneider-electric/dpn10-10/box-abs-103x103x56mm/dp/161706 has worked well.
For a laser-cut card holder front-end, see the dxf files in this repository's hardware/ directory. These will need M2.5 nylon nuts to hold the stack of laser cut acrylic together through to the front cover of the reader.
We have typically used arcade microswitch buttons with LED. " Ultralux RGB Illuminated Arcade Button " from Arcade World UK. https://www.arcadeworlduk.com/products/ultralux-rgb-illuminated-arcade-button.html

For doorbot roles, we typically build the readers into standard socket boxes with blanking plates. Exterior card readers will need a suitably watertight enclosure with a watertight button.
Drilling a standard EN 60670-1 thermosetting plastic blanking plate to fit LED and buttons makes them prone to cracking. Start with a tiny pilot drill, ideally in a drill press, and work up sizes. Fortunately, they are cheap and very easy to come by (wilko,screwfix,B&Q).
RJ45 termination to a modular jack is typically within the enclosure rather than external as on most of the acnode role readers.
PN532 RFID reader module: Set to HSS mode. Elechouse V4. Order direct from Elechouse. Do not skimp on ordering from aliexpress or ebay, because there are huge numbers of clones with AWFUL antenna matching circuitry.
