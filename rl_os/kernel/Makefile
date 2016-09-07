# Declaration of variables
CC = lcc
LD = link
CC_FLAGS = "-I./include"
AS = asm


 # File names
IMAGE = image
SOURCES = $(wildcard *.c)
ASM_SOURCES = $(wildcard *.s)
OBJECTS = $(SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
# Main target
$(IMAGE): $(OBJECTS) $(ASM_OBJECTS)
	$(CC) -Wo"-kernel" $(OBJECTS) $(ASM_OBJECTS) -o $(IMAGE)

# To obtain object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

%.o: %.s
	$(AS) $< $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS) $(ASM_OBJECTS)

