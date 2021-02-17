APPLICATION = sensors

BOARD ?= nrf52840dongle
PORT ?= /dev/ttyACM0

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += xtimer

# Add the required modules here
USEMODULE += bmp280_spi

CFLAGS+= -DBMX280_PARAM_CS=GPIO_PIN\(0,17\) 

RIOTBASE ?= $(CURDIR)/../../RIOT

include $(RIOTBASE)/Makefile.include
