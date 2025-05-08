CROSS_COMPILE ?= arm-linux-gnueabihf-
TARGET ?= uart

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

# 数学库：在函数中使用除法运算时必须要使用该库
# -L 选项指定库所在的目录
LIBPATH := -lgcc -L /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/lib/gcc/arm-linux-gnueabihf/4.9.4

INCDIRS := nxp \
			bsp/clk \
			bsp/led \
			bsp/delay \
			bsp/beep \
			bsp/gpio \
			bsp/key \
			bsp/exit \
			bsp/int \
			bsp/epit \
			bsp/key_filter \
			bsp/uart

SRCDIRS := project \
			bsp/clk \
			bsp/led \
			bsp/delay \
			bsp/beep \
			bsp/gpio \
			bsp/key \
			bsp/exit \
			bsp/int \
			bsp/epit \
			bsp/key_filter \
			bsp/uart

# patsubst 函数对 INCDIRS 里的每个目录添加 -I 前缀，从而形成编译器的头文件搜索路径选项
# Makefile 语法要求，在文件夹中搜索文件时，需要使用 -I 选项
# 因此，展开结果为 INCLUDE := -I imx6ul -I bsp/clk -I bsp/led -I bsp/delay
INCLUDE := $(patsubst %, -I %, $(INCDIRS))

# 在 SRCDIRS 中查找所有后缀为 .S 的文件，这样获得的文件前面会带上绝对路径
# 这里 SFILES 等同于 project/start.S
SFILES := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

# notdir 会去除文件前面的路径，只保留文件名
# 这里 SFILENDIR 等同于 start.S
SFILENDIR := $(notdir $(SFILES))
CFILENDIR := $(notdir $(CFILES))

# 将 SFILENDIR 中的 .S 文件替换为 obj/ 目录中的 .o 文件
# 这里 SOBJS 等同于 start.o
# $(SFILENDIR:.S=.o) 将 SFILENDIR 中所有文件后缀为 .S 的替换为 .o
SOBJS := $(patsubst %, obj/%, $(SFILENDIR:.S=.o))
COBJS := $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
OBJS := $(SOBJS)$(COBJS)

# 指定搜索源文件的路径，也就是 SRCDIRS 中的目录
VPATH := $(SRCDIRS)

# 把 clean 标记为伪目标，保证 clean 目标不会和同名文件产生冲突
.PHONY:
	clean

$(TARGET).bin : $(OBJS)
# $^ 为自动化变量，表示所有依赖项，这里等同于 start.o 和 main.o
	$(LD) -TLinkfile.lds -o $(TARGET).elf $^ $(LIBPATH)
# 自动化变量 $@ 表示目标文件，这里等同于 bsp.bin
	$(OBJCOPY) -O binary -S $(TARGET).elf $@
	$(OBJDUMP) -D -m arm $(TARGET).elf > $(TARGET).dis

$(SOBJS) : obj/%.o : %.S
# 自动化变量 $@ 表示目标文件，这里等同于 start.o
# 自动化变量 $< 表示依赖文件，这里等同于 start.s
# -fno-builtin 参数表示不使用内建函数，例如 C 库中已经定义了 putc() 函数
# 如果用户定义名为 putc() 的函数，则在编译时会报错：warning: conflicting types for built-in function ‘putc’
# 使用该参数后，用户就可以实现自己的类似 putc() 这样的函数
	$(CC) -Wall -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

$(COBJS) : obj/%.o : %.c
	$(CC) -Wall -nostdlib -fno-builtin -c -O2 $(INCLUDE) -o $@ $<

clean:
	rm -rf $(TARGET).elf $(TARGET).dis $(TARGET).bin $(COBJS) $(SOBJS) load.imx
