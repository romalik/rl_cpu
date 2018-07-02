# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = "-I../../rootfs/lib/include"
AS = asm


 # File names
CMD_SOURCES = $(wildcard *.c)
LIBS = $(wildcard ../../rootfs//lib/*.o)


BINS = $(CMD_SOURCES:.c=)

# Main target
all: $(BINS)

%: %.c
	$(CC) -Wl"-text" -Wl"0x0000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x0000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $< -o $@

# To remove generated files
clean:
	rm -f $(BINS)

install: $(BINS)
	cp $(BINS) ../../rootfs/bin
