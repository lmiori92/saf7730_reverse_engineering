/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Lorenzo Miori (C) 2016 [ 3M4|L: memoryS60<at>gmail.com ]

    Version History
        * 1.0 initial

*/

#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include <stdio.h>

/** Reception callback function */
typedef void (*t_uart_cb)(uint8_t byte);

void uart_callback(t_uart_cb cb);
void uart_putchar(char c, FILE *stream);
void uart_putstring(char *str);
char uart_getchar(FILE *stream);

void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

extern FILE uart_output;
extern FILE uart_input;

#endif /* _UART_H_ */
