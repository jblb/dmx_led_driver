BOARD_TAG    = uno
ARDUINO_PORT = /dev/ttyACM0
ARDUINO_LIBS = Tlc5940
## ARDUINO_DIR  = /home/jerome/Documents/arduino-1.0.1
## ARDUINO_SKETCHBOOK = /home/jerome/Documents/sketchbook

AVRDUDE = $(ARDUINO_DIR)/hardware/tools/avrdude
AVRDUDE_CONF = $(ARDUINO_DIR)/hardware/tools/avrdude.conf

include ../Arduino.mk

