leds.bin : leds.S
# -g 表示产生调试信息，GDB 可以根据这些信息进行调试
# -c 表示仅编译源文件，但不进行链接
# -o 指定输出文件的名称，这里指定的名称为 leds.o
	arm-linux-gnueabihf-gcc -g -c leds.S -o leds.o
# -Ttext 指定链接地址
	arm-linux-gnueabihf-ld -Ttext 0x87800000 leds.o -o leds.elf
# -O 指定输出格式，binary 表示输出文件的格式为二进制
# -S 表示不要复制源文件中的重定位信息和符号信息
# -g 表示不要复制源文件中的调试信息
	arm-linux-gnueabihf-objcopy -O binary -S -g leds.elf leds.bin
# -D 表示反汇编所有的段
	arm-linux-gnueabihf-objdump -D leds.elf > leds.dis

clean:
	rm -rf *.o leds.bin leds.elf leds.dis
