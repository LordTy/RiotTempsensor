# RiotTempsensor
This is a simple temperature sensor node using RIOT-os, a bmp280 and a NRF52840 dongle.

For more information on how this project came to be see tbnk.nl

# Compile instructions
To compile these projects you need a Working RIOT-os install.  

Point the RIOTBASE variable in the makefiles to the base RIOT folder

Then create your own networksettings.inc.mk based on the example.  

The Node folder contains a the firmware for the temperature node.

The BorderRouter contains the settings for the Border router.

# Hardware/Network Setup.
You need 1 border router and N(>1) temperature nodes.  
The target host needs to be able to accept temperatures on:

    coap://[DEFAULT_HOST_IP]:5863/temp.  
