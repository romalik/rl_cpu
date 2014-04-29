ASM=../assembler/b.sh
SOURCES=int_test.as tty_test.as asm_test.as call_test.as cmd.as
IMAGES=int_test.img tty_test.img asm_test.img call_test.img cmd.img



all: $(SOURCES) $(IMAGES)

clean:
	rm *.img

call_test.img: call_test.as
	$(ASM) call_test.as -o call_test.img
    
tty_test.img: tty_test.as
	$(ASM) tty_test.as -o tty_test.img

asm_test.img: asm_test.as
	$(ASM) asm_test.as -o  asm_test.img

int_test.img: int_test.as
	$(ASM) int_test.as -o  int_test.img

cmd.img: cmd.as
	$(ASM) cmd.as -o  cmd.img


