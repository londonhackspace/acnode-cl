This is yet another board. IT IS NOT YET READY FOR USE!

It's basically a backport of the design I came up with for a Nucleo-based version. Key features include:
* An LED on the board for debugging
* A button on the board for debugging
* Support for RS-422 reader connection
* SPI Flash instead of SD Card
* Opto-isolated tool on input
* Trasistor-buffered door release with ferrite beads

Things to fix before the next version:

* Launchpad ethernet connector fouls the shield
* LED is... rather bright...
* A jumper on the ISL3176 ~RE line would allow a single board to support 5V and 12V readers
* Footprints for 3.81mm connectors are too tight - holes too small
