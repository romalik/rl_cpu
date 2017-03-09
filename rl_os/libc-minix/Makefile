# Declaration of variables
CC = lcc -d1000
LD = link
CC_FLAGS = -I./include -D__STDC__=1 -D_POSIX_SOURCE -D_EM_WSIZE=2 -DPRIVATE=static -D_MINIX 
AS = asm


 # File names
LIB = stdlib
ASM_SOURCES = $(wildcard s/*.s)
SOURCES = $(wildcard c/*/*.c)
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

