# CROSS_COMPILE 为自定义变量，其等同于 arm-linux-gnueabihf-
# ?= 为条件赋值运算符，这里就是检查 CROSS_COMPILE 之前是否已经被赋值
# 如果没有则将 arm-linux-gnueabihf- 赋给 CROSS_COMPILE
CROSS_COMPILE ?= arm-linux-gnueabihf-
NAME ?= ledc

# CC 为默认变量，:= 为立即赋值运算符，这里表示将 $(CROSS_COMPILE)gcc 赋给 CC
# 这里的 $(CROSS_COMPILE)gcc 等同于 arm-linux-gnueabihf-gcc
# 同时，后续就算 $(CROSS_COMPILE)gcc 发生变化，CC 也不会改变
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

OBJS := start.o main.o

$(NAME).bin : $(OBJS)
# $^ 为自动化变量，表示所有依赖项，这里等同于 start.o 和 main.o
	$(LD) -TLinkfile.lds -o $(NAME).elf $^
# -O 表示以制定格式输出，binary 表示输出为二进制文件
# -S 表示不要赋值符号表和重定位信息
	$(OBJCOPY) -O binary -S $(NAME).elf $@
# -D 表示反汇编所有的段
# -m 制定目标架构，arm 表示 ARM 架构
	$(OBJDUMP) -D -m arm $(NAME).elf > $(NAME).dis

%.o : %.s
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<

%.o : %.S
# -Wall 表示打开所有警告信息
# -nostdlib 表示不使用标准库
# -c 表示只进行编译，不进行链接
# 自动化变量 $@ 表示目标文件，这里等同于 start.o
# 自动化变量 $< 表示依赖文件，这里等同于 start.s
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<

%.o : %.c
	$(CC) -Wall -nostdlib -c -O2 -o $@ $<

clean:
	rm -rf *.o $(NAME).elf $(NAME).dis $(NAME).bin load.imx
