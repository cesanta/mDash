# Builds the firmware in a container, no local IDF installation needed
CWD = $(realpath $(CURDIR))
MNT = $(realpath $(CURDIR)/..)
TARGET ?= esp32

all: build

build: $(wildcard main/*)
	podman run --rm -v $(MNT):$(MNT) -w $(CWD) espressif/idf idf.py set-target $(TARGET)
	podman run --rm -v $(MNT):$(MNT) -w $(CWD) espressif/idf idf.py build

flash:
	(cd build && esptool --port /dev/cu.usb* write-flash @flash_args)

clean:
	rm -rf build

.PHONY: all build clean
