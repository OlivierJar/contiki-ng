CONTIKI_PROJECT = udp-client udp-server udp-relay uart-test
all: $(CONTIKI_PROJECT)
TARGET=cc26x0-cc13x0
MODULES += os/services/shell 
MAKE_MAC = MAKE_MAC_TSCH
CONTIKI=../..
include $(CONTIKI)/Makefile.include
