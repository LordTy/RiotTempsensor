RIOTBASE ?= $(CURDIR)/../../../RIOT

#include wireless network parameters
include ../Node/networksettings.inc.mk

#Settings to change on the border router example
BOARD = nrf52840dongle
UPLINK = cdc-ecm

include $(RIOTBASE)/examples/gnrc_border_router/Makefile

install-service:
	envsubst < riot-networking.service.template > riot-networking.service
	cp riot-networking.service /lib/systemd/system/
	systemctl enable riot-networking
	systemctl start riot-networking