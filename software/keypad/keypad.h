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

#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "keypad_buttons.h"

/**< Enumeration of button event */
typedef enum e_key_event_
{
    KEY_NONE,
    KEY_CLICK,
    KEY_HOLD
} e_key_event;

typedef struct
{

    bool         input;
    uint8_t      debounce;
    bool         latches;
    e_key_event  buttons;

} t_keypad;

void keypad_init(void);
e_key_event keypad_clicked(e_key key);
void keypad_reset_input(void);
void keypad_set_input(e_key key, bool value);
bool keypad_get_input(e_key key);
void keypad_periodic(bool flag_debounce);

#endif /* _KEYPAD_H_ */
