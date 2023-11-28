/**
 * @file uart.h
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init();
uint8_t uart_get_char();
uint8_t uart_put_chars( const char* byte, uint8_t length );
bool uart_end_of_file();

#endif // UART_H
