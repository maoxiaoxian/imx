@ 汇编驱动 LED
.global _start

@ 主函数
_start:
    @ 时钟使能
    ldr r0, =0x020C4068 @ CCM_CCGR0
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C406C @ CCM_CCGR1
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C4070 @ CCM_CCGR2
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C4074 @ CCM_CCGR3
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C4078 @ CCM_CCGR4
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C407C @ CCM_CCGR5
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    ldr r0, =0x020C4080 @ CCM_CCGR6
    ldr r1, =0xFFFFFFFF @ 寄存器全部位置 1
    str r1, [r0]

    @ 设置 GPIO 复用：GPIO1_IO03 的功能选择为 GPIO1_IO03
    ldr r0, =0x020E0068 @ 将寄存器 IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03 的地址保存至 R0 寄存器
                        @ 该寄存器的 MUX_MODE[3:0] 字段选择 GPIO1_IO03 的功能
    ldr r1, =0x00000005 @ MUX_MODE[3:0] 设置为 0b0101（0x5） 表示 GPIO1_IO03 用作通用 GPIO
    str r1, [r0]        @ 将 R1 中的值写入 R0 中的地址，即将 0x5 写入 0x020E0068

    @ 设置 GPIO1_IO03 的电气特性
    @ 寄存器 IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 设置 GPIO1_IO03 的电气特性
    @ 该寄存器中各字段的含义，以及将要设置的值的含义如下：
    @ SRE[0]：压摆率，0b0，低压摆率
    @ DSE[5:3]：IO 驱动能力，0b110，R0/6
    @ SPEED[7:6]：IO 速率，0b10，100MHz
    @ ODE[11]：开漏输出使能，0b0，禁用
    @ PKE[12]：状态保持器使能，0b1，使能
    @ PUE[13]：使用上拉/下拉 or 状态保持器控制引脚，0b0，使用上拉/下拉方式控制
    @ PUS[15:14]：上拉/下拉电阻值，0b00，下拉
    @ HYS[16]：使能迟滞比较器，0b0，禁用
    @ 因此，最终要写入寄存器的值为：0x10B0
    ldr r0, =0x020E02F4 @ 将寄存器 IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 的地址保存至 R0 寄存器
    ldr r1, =0x000010B0 @ 将要写入寄存器中的值保存至 R1 寄存器
    str r1, [r0]        @ 将值 0x000010B0 写入 IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03 寄存器

    @ GPIO1_IO03 工作模式选择：输入 or 输出
    @ 寄存器 GPIO1_GDIR 控制 GPIO1 组内各引脚为输入还是输出，对应的位置 1 表示该引脚为输出
    ldr r0, =0x0209C004 @ 将寄存器 GPIO1_GDIR 的地址保存至 R0
    ldr r1, =0x00000008 @ 将 IO03 引脚设置为输出
    str r1, [r0]        @ 将 0x00000008 写入 GPIO1_GDIR 寄存器

    @ 控制 GPIO1_IO03 进行输出
    @ 寄存器 GPIO1_DR 控制 GPIO1 组中各引脚的输入/输出值
    @ 向对应的位写 1，会触发对应的位进行输出，读取则会获取该引脚的当前值
    ldr r0, =0x0209C000 @ 将寄存器 GPIO1_DR 的地址保存至 R0
    ldr r1, =0x00000000 @ 控制引脚输出（写 1 还是写 0 要根据电路实现而定）
    str r1, [r0]        @ 将 0x00000000 写入 GPIO1_DR 寄存器

@ 程序循环
loop:
    b loop
