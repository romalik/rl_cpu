# Declaration of variables
CC = lcc -d1000
LD = link
CC_FLAGS = -I./include -D__HAS_NO_FLOATS__
AS = asm


 # File names
LIB = stdlib
ASM_SOURCES = $(wildcard s/*.s)
SOURCES = $(wildcard c/*.c)
#SOURCES += $(wildcard c/libcurses/*.c)
#SOURCES += $(wildcard c/curses_minix/*.c)
OBJECTS = $(SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
# Main target
all: $(OBJECTS) $(ASM_OBJECTS)
	cp $(OBJECTS) $(ASM_OBJECTS) ../rootfs/lib/

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

%.o: %.s
	$(AS) $< $@
# To remove generated files
clean:
	rm -f $(ASM_OBJECTS) $(OBJECTS)

