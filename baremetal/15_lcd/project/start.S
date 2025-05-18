.global _start

_start:
    ldr pc, =Reset_Handler     @ 复位中断
    ldr pc, =Undefined_Handler @ 未定义中断
    ldr pc, =SVC_Handler       @ SVC 中断
    ldr pc, =PreAbort_Handler  @ 预取指令终止中断
    ldr pc, =DataAbort_Handler @ 数据终止中断
    ldr pc, =NotUsed_Handler   @ 未使用中断
    ldr pc, =IRQ_Handler       @ IRQ 中断
    ldr pc, =FIQ_Handler       @ FIQ 中断

@ 复位中断
Reset_Handler:
    cpsid i @ 屏蔽 IRQ 中断

    @ 关闭 I-Cache 和 D-Cache（采用读改写的方式操作 SCTLR 寄存器）
    mrc p15, 0, r0, c1, c0, 0 @ 将 CP15 的 C1 寄存器（SCTLR）的值读取至 R0 寄存器
    bic r0, r0, #(0x1 << 12)  @ 清除 C1 寄存器的 bit12（I 位），关闭 I-Cache
    bic r0, r0, #(0x1 << 2)   @ 清除 C1 寄存器的 bit2（C 位），关闭 D-Cache
    bic r0, r0, #(0x1 << 1)   @ 清除 C1 寄存器的 bit1 (A 位)，关闭对齐
    bic r0, r0, #(0x1 << 11)  @ 清除 C1 寄存器的 bit11（Z 位），关闭分支预测
    bic r0, r0, #(0x1 << 0)   @ 清除 C1 寄存器的 bit0（M 位），关闭 MMU
    mcr p15, 0, r0, c1, c0, 0 @ 将 R0 寄存器中的值写回 CP15 协处理器的 C1 寄存器（SCTLR）

#if 0
    @ 汇编测试：设置中断向量表偏移
    ldr r0, =0x87800000

    dsb
    isb

    mcr p15, 0, r0, c12, c0, 0

    dsb
    isb
#endif

    @ 设置各个模式下的堆栈指针，这里需要注意 IMX 的堆栈是向下增长的
    @ 堆栈指针地址必须保证 4 字节对齐，DDR 范围：0x80000000 ~ 0x9FFFFFFF

    @ 进入 IRQ 模式
    mrs r0, cpsr @ 将 CPSR 寄存器中的值读取至 R0 寄存器
    bic r0, r0, #0x1F @ 清空 R0 寄存器的低 5 位，即清空 CPSR 寄存器的 M[4:0]
    orr r0, r0, #0x12 @ 设置 IRQ 模式，即 M[4:0] = 0b10010
    msr cpsr, r0      @ 将 R0 中的值写回 CPSR 寄存器，即设置当前 PE 模式为 IRQ 模式
    ldr sp, =0x80600000 @ 设置 IRQ 模式的栈起始地址，大小为 2MBytes，地址范围 0x80600000 ~ 0x80400000（向下增长）

    @ 进入 SYS 模式
    mrs r0, cpsr @ 读取 CPSR 寄存器
    bic r0, r0, #0x1F @ 清空 CPSR 寄存器的 M[4:0]
    orr r0, r0, #0x1F @ 设置进入 SYS 模式，即 M[4:0] = 0b11111
    msr cpsr, r0      @ 将 R0 中的值写回 CPSR 寄存器
    ldr sp, =0x80400000 @ 设置 SYS 模式的栈起始地址，大小为 2MBytes，地址范围 0x80400000 ~ 0x80200000（向下增长）

    @ 进入 SVC 模式
    mrs r0, cpsr @ 读取 CPSR 寄存器
    bic r0, r0, #0x1F @ 清空 CPSR 寄存器的 M[4:0]
    orr r0, r0, #0x13 @ 设置进入 SVC 模式，即 M[4:0] = 0b10011
    msr cpsr, r0      @ 将 R0 中的值写回 CPSR 寄存器
    ldr sp, =0x80200000 @ 设置 SVC 模式的栈起始地址，大小为 2MBytes，地址范围 0x80200000 ~ 0x80000000（向下增长）

    cpsie i @ 使能 IRQ 中断

#if 0
    @ 使能 IRQ 中断
    mrs r0, cpsr @ 将 CPSR 寄存器的值读取至 R0 寄存器
    bic r0, r0, #0x80 @ 清空 CPSR 寄存器的第 7 位，即 I 标志位，表示允许 IRQ 中断
    msr cpsr, r0 @ 将 R0 中的值写回 CPSR 寄存器
#endif

    @ 跳转至 main() 函数执行
    b main

@ 未定义中断
Undefined_Handler:
    ldr r0, =Undefined_Handler
    bx r0

@ SVC 中断
SVC_Handler:
    ldr r0, =SVC_Handler
    bx r0

@ 预取指令终止中断
PreAbort_Handler:
    ldr r0, =PreAbort_Handler
    bx r0

@ 数据终止中断
DataAbort_Handler:
    ldr r0, =DataAbort_Handler
    bx r0

@ 未使用中断
NotUsed_Handler:
    ldr r0, =NotUsed_Handler
    bx r0

@ IRQ 中断
IRQ_Handler:
    push {lr}         @ 保存 LR 寄存器的中的地址（返回地址）
    push {r0-r3, r12} @ 保存 R0~R3 以及 R12 寄存器

    mrs r0, spsr @ 读取 SPSR 寄存器
    push {r0}    @ 保存 SPSR 寄存器的值

    mrc p15, 4, r1, c15, c0, 0  @ 将 CP15 的 C0 寄存器的值保存至 R1 寄存器
                                @ CP15 的 C15 寄存器为 CBAR 寄存器，保存了 GIC 寄存器组的基地址
    add r1, r1, #0x2000 @ GIC 寄存器组的基地址加上 0x2000，即获取 GIC 控制器 CPU 接口寄存器组的基地址
    ldr r0, [r1, #0xC]   @ GIC CPU 接口寄存器组的基地址加上 0xC 即 GICC_IAR 寄存器
                        @ GICC_IAR 寄存器保存当前中断的中断号，需要根据该中断号调用对应的中断服务函数
    push {r0, r1}

    cps #0x13 @ 进入 SVC 模式，允许其他中断产生

    push {lr} @ 保存 SVC 模式的 LR 返回地址寄存器
    ldr r2, =sys_irq_handler    @ 加载 C 语言的中断处理函数入口地址到 R2 寄存器
                                @ 函数的前 4 个参数默认保存在 R0~R3 寄存器中
    blx r2 @ 跳转至该函数执行

    @ 中断服务函数执行完成，恢复现场
    pop {lr} @ 获取返回地址
    cps #0x12 @ 进入 IRQ 模式
    pop {r0, r1}
    str r0, [r1, #0x10] @ 中断执行完成，写 EOIR（End of Interrupt）

    pop {r0}
    msr spsr_cxsf, r0 @ 恢复 SPSR 寄存器

    pop {r0-r3, r12} @ R0~R3 以及 R12 寄存器出栈
    pop {lr}
    subs pc, lr, #0x4 @ 将 LR - 0x4 赋给 PC（下一条指令的地址）

FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx r0
