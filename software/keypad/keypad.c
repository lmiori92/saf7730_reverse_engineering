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

#include "keypad.h"

#include <stdbool.h>

static t_keypad keypad[NUM_BUTTONS];

void keypad_init(void)
{
    uint8_t i = 0;

    /* Init keypad */
    for (i = 0; i < NUM_BUTTONS; i++)
    {
        keypad[i].input    = false;
        keypad[i].buttons  = false;
        keypad[i].latches  = false;
        keypad[i].debounce = 0;
    }
}

void keypad_reset_input(void)
{
    uint8_t i = 0;

    for (i = 0; i < NUM_BUTTONS; i++)
    {
        keypad[i].input    = false;
    }
}

void keypad_set_input(e_key key, bool value)
{
    keypad[key].input = value;
}

bool keypad_get_input(e_key key)
{
    return keypad[key].input;
}

e_key_event keypad_clicked(e_key key)
{
    return keypad[key].buttons;
}

/* Read the keypad, apply debounce to inputs and detect the rising edge */
void keypad_periodic(bool flag_debounce)
{

    uint8_t i = 0;

    for (i = 0; i < NUM_BUTTONS; i++)
    {
        /* the event shall be consumed within a cycle */
        keypad[i].buttons = KEY_NONE;

        /* debounce the raw input */
        if ((flag_debounce == true) && (keypad[i].debounce < KEY_DEBOUNCE_HOLD))
        {
            /* flag available, count up */
            keypad[i].debounce++;
        }
        else
        {
            /* flag not available, wait next round */
        }

        if (keypad[i].latches == false && keypad[i].input == true)
        {
            /* rising edge */
            keypad[i].debounce = 0;
        }
        else if (keypad[i].latches == true && keypad[i].input == true)
        {

            if (keypad[i].debounce == KEY_DEBOUNCE_HOLD)
            {
                /* button stayed high for long enough for a hold event */
                keypad[i].buttons = KEY_HOLD;
                /* prevent overflow */
                keypad[i].debounce = KEY_DEBOUNCE_HOLD + 1U;
            }
            else
            {
                /* not yet triggered or timeout */
            }
        }
        else if (keypad[i].latches == true && keypad[i].input == false)
        {
            if (keypad[i].debounce >= KEY_DEBOUNCE_CLICK && keypad[i].debounce < KEY_DEBOUNCE_HOLD)
            {
                /* click event */
                keypad[i].buttons = KEY_CLICK;
            }
            else
            {
                /* either not enough for a click or hold event */
            }

            keypad[i].debounce = 0;
        }

        keypad[i].latches = keypad[i].input;

    }

}
