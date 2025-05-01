.global _start

_start:
    @ 处理器进入 SVC 模式
    mrs r0, cpsr      @ 将当前程序状态寄存器 CPSR 中的值读取至 R0 通用寄存器
    bic r0, r0, #0x1F @ 清空 CPSR.M[4:0]，即 CPSR 寄存器的低 5 位
    orr r0, r0, #0x13 @ 将 R0 或上 0x13（0b10011），即 M[4:0] 设置为 SVC 模式
    msr cpsr, r0      @ 将 R0 中的值写回 CPSR 寄存器，即设置处理器为 SVC 模式

    ldr sp, =0x80200000 @ 设置栈指针，栈内存向下增长，因此栈大小为：0x80200000 - 0x80000000 = 2MBytes
    b main
