#ifndef _UART_H
#define _UART_H

#include "nxp.h"

// 函数声明
void uart_init(void);
void uart_io_init(void);
void uart_disable(UART_Type *base);
void uart_enable(UART_Type *base);
void uart_soft_reset(UART_Type *base);
void uart_set_baudrate(UART_Type *base, uint32_t baudrate, uint32_t clk);

void put_char(char c);
void put_string(char *str);
char get_char(void);

// 下面这个函数用于防止编译器报错
void raise(int sig_nr);

#endif /* _UART_H */
