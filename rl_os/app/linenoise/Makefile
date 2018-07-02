# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = -I../../rootfs/lib/include -I./
AS = asm


 # File names
SRCS = $(wildcard *.c)
LIBS = $(wildcard ../../rootfs//lib/*.o)


BINS = lne

# Main target
all: $(BINS)

# To obtain object files
lne: $(SRCS)
	$(CC) -Wl"-text" -Wl"0x0000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x0000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $(SRCS) -o lne

# To remove generated files
clean:
	rm -f $(BINS)

install: $(BINS)
	cp $(BINS) ../../rootfs/bin
