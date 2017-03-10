# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = "-I../rootfs/lib/include"
AS = asm


 # File names
SOURCES = $(wildcard cmd/*.c)
LIBS = $(wildcard ../rootfs//lib/*.o)
BINS = $(SOURCES:.c=)
# Main target
all: $(BINS)
	cp $(BINS) ../rootfs/bin
# To obtain object files
%: %.c
	$(CC) -Wl"-text" -Wl"0x8000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x8000" $(CC_FLAGS) $(LIBS) $< -o $@

# To remove generated files
clean:
	rm -f $(BINS)

