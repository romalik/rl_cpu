# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = "-I../rootfs/lib/include"
AS = asm


 # File names
CMD_SOURCES = $(wildcard cmd/*.c)
LIBS = $(wildcard ../rootfs//lib/*.o)
UE_SOURCES = ue/ue.c ue/term-ansi.c
#SH_SOURCES = $(wildcard sh/*.c)


BINS = $(CMD_SOURCES:.c=)
BINS += ue/ue
#BINS += sh/shv7

# Main target
all: $(BINS)
	cp $(BINS) ../rootfs/bin

ue/ue:
	$(CC) -Wl"-text" -Wl"0x8000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x8000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $(UE_SOURCES) -o ue/ue


sh/msh:
	$(CC) -Wl"-text" -Wl"0x8000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x8000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $(SH_SOURCES) -o sh/msh


# To obtain object files
%: %.c
	$(CC) -Wl"-text" -Wl"0x8000" -Wl"-mode" -Wl"1"  -Wl"-data" -Wl"0x8000"  -Wl"-strip"  $(CC_FLAGS) $(LIBS) $< -o $@

# To remove generated files
clean:
	rm -f $(BINS)

