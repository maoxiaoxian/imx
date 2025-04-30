# objs 为自定义变量，其值为 start.o main.o
objs := start.o main.o

# ledc.bin 为要生成的目标文件，其依赖项为 $(objs)
# $(objs) 引用 objs 变量，即 $(objs) 等同于 start.o main.o
ledc.bin : $(objs)
# 根据 Linkfile.lds 中定义的规则，链接 $^ 所代表的文件，生成 ledc.elf 可执行文件
# $^ 为自动化变量，表示所有依赖项，这里等同于 start.o 和 main.o
# -o 指定要生成的文件名称，这里为 ledc.elf
	arm-linux-gnueabihf-ld -TLinkfile.lds -o ledc.elf $^
# 将源文件转换为目标文件
# -O 指定要生成的文件格式，binary 表示二进制格式的文件
# -S 表示不要复制符号表和重定位信息
# $@ 为自动化变量，表示目标文件，这里等同于 ledc.bin
	arm-linux-gnueabihf-objcopy -O binary -S ledc.elf $@
# 对源文件进行反汇编，这里的源文件为 ledc.elf
# -D 表示反汇编所有的段
# -m 指定目标架构，arm 表示目标架构为 ARM
# 反汇编生成的信息由 > 指定保存至 ledc.dis 中
	arm-linux-gnueabihf-objdump -D -m arm ledc.elf > ledc.dis

# 目标文件为 %.o，即要生成的文件，其依赖项为 %.s
# % 为通配符，表示所有符合规则的文件，例如，%.s 表示所有后缀为 .s 的文件
%.o : %.s
# -Wall 表示开启所有的警告信息
# -nostdlib 表示不使用标准库
# -c 表示只进行编译，不要进行链接
# -O2 表示优化等级为 2
# -o 指定要是生成的文件
# $@ 自动化变量代表目标文件，这个例程中就表示 main.o
# $< 自动化变量代表当前规则的第一个依赖文件，在模式规则中，指的是具体匹配到的依赖文件；在普通规则里，就是依赖列表中的首个文件
# 这里 $< 就代表 main.c
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<

%.o : %.S
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<

%.o : %.c
	arm-linux-gnueabihf-gcc -Wall -nostdlib -c -O2 -o $@ $<

# clean 规则，在终端中执行 make clean 时就会触发该规则，执行其定义的命令
clean:
	rm -rf *.o ledc.bin ledc.elf ledc.dis load.imx
