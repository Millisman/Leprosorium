# For package building, you will need to pass RELEASE=1 to make
RELEASE	?= 0

.PHONY: build-examples

all:	build-examples

build-examples:
	$(MAKE) -C examples RELEASE=$(RELEASE)

clean:
	$(MAKE) -C examples clean
	$(MAKE) -C examples/parts clean
