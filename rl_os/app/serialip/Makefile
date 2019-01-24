# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = -I../../rootfs/lib/include -I./ -I./uip/
AS = asm


 # File names
SRCS = $(wildcard *.c)
SRCS += $(wildcard uip/*.c)
LIBS = $(wildcard ../../rootfs//lib/*.o)


BINS = uip_test

# Main target
all: $(BINS)

# To obtain object files
uip_test: $(SRCS)
	$(CC) -Wl"-text" -Wl"0x0000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x0000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $(SRCS) -o uip_test

# To remove generated files
clean:
	rm -f $(BINS)

install: $(BINS)
	cp $(BINS) ../../rootfs/bin
