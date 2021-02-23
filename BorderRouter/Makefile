RIOTBASE ?= $(CURDIR)/../../../RIOT

#include wireless network parameters
include ../Node/networksettings.inc.mk

#Settings to change on the border router example
BOARD = nrf52840dongle
UPLINK = cdc-ecm

include $(RIOTBASE)/examples/gnrc_border_router/Makefile

riot-networking.service: riot-networking.service.template
	envsubst < riot-networking.service.template > riot-networking.service

.PHONY:install-service
install-service: riot-networking.service	
	cp riot-networking.service /etc/systemd/system/
	systemctl enable --now riot-networking