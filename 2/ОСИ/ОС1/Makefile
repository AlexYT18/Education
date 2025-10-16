all: bootsect.bin kernel.bin

bootsect.bin: bootsect.asm
	yasm -f bin -o bootsect.bin bootsect.asm

kernel.o: kernel.cpp
	gcc -m32 -ffreestanding -fno-pie -fno-builtin -fno-stack-protector -nostdlib -c kernel.cpp -o kernel.o

kernel.bin: kernel.o
	ld -m elf_i386 -Ttext 0x10000 -o kernel.elf kernel.o
	objcopy -O binary kernel.elf kernel.bin

run: all
	qemu-system-i386 -fda bootsect.bin -fdb kernel.bin

clean:
	rm -f *.bin *.o *.elf