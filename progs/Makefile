ASM=../assembler/b.sh
SOURCES=tty_test.as asm_test.as call_test.as str_test.as
IMAGES=tty_test.img asm_test.img call_test.img str_test.img



all: $(SOURCES) $(IMAGES)

clean:
	rm *.img

call_test.img: call_test.as
	$(ASM) call_test.as sim > call_test.img
    
tty_test.img: tty_test.as
	$(ASM) tty_test.as sim > tty_test.img

asm_test.img: asm_test.as
	$(ASM) asm_test.as sim > asm_test.img

str_test.img: str_test.as
	$(ASM) str_test.as sim > str_test.img


