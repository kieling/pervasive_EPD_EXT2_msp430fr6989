# Pervasive Displays EPD EXT2 port to the MSP430FR6989

Port of the Pervaside Displays EPD EXT2 code to the MSP430FR6989.
Sofar only single driver functionality is working.
PDI APPs build is too big to fit the SRAM on the MSP430FR6989. Might work if everything is pushed to the FRAM.

Tested with a 1,9" display.

To build:
  - Import the project into CSSv6
  - Select the eTC_G2_Size_Aurora_Mb configuration.
