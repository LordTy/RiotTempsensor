RIOTBASE ?= $(CURDIR)/../../../RIOT

#include wireless network parameters
include ../Node/networksettings.inc.mk

#Settings to change on the border router example
BOARD = nrf52840dongle
UPLINK = cdc-ecm

include $(RIOTBASE)/examples/gnrc_border_router/Makefile
