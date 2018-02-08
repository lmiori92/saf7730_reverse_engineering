/*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.

*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.

*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Lorenzo Miori (C) 2017-2018 [ 3M4|L: memoryS60<at>gmail.com ]
*
*/

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#include "i2c_master.h"
#include "timer.h"

#include "saf7730.h"

#include "uart.h"

#include "keypad/keypad.h"

#include <avr/power.h>
#include <avr/sleep.h>

/* #define DEBUG   1 */

#ifdef DEBUG
#define DEBUG_EXEC(x) do { \
                        x  \
                      } while(0);
#else
#define DEBUG_EXEC(x) while(0) { x };
#endif

static uint8_t current = 0;
static uint8_t new = 0;
#ifdef DEBUG
static uint32_t itest = 0;
static bool debug_isr = false;
#endif

/** Definition of the i2c inactivity timeout for going to sleep */
#define SLEEP_AFTER_MS  (30000U)
/** Definition of the minimum i2c inactivity time to wait before starting a transfer on the bus */
#define TX_AFTER_MS     (6U)

ISR(TIMER0_COMPA_vect)
{
    /* increment the timer variable */
    milliseconds_since_boot += 1;
}

ISR(PCINT1_vect)
{
    /* reset timer if i2c busy */
    TIMER_RESET(SOFT_TIMER_2);
    TIMER_RESET(SOFT_TIMER_3);
#ifdef DEBUG
    DEBUG_EXEC( { itest++; if (itest > 100) { debug_isr = true; } } );
#endif
}

ISR(BADISR_vect)
{
    DEBUG_EXEC( { uart_putstring("bad isr!"); } );
    while(1);
}

static void saf7730_input_mux_switch(uint8_t source)
{
    uint8_t buf[7];
    buf[0] = (uint8_t)(COMM_SAF7730_ADDR << 1U);
    buf[1] = 0x0D;
    buf[2] = 0x00;
    buf[3] = 0x6A;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = source;

    /* start the i2c transfer and wait for it */
    i2c_transfer_set_data(buf, sizeof(buf));
    i2c_transfer_start();
    i2c_transfer_successful();
}

static void source_select_next(void)
{
    if (current == new)
    {
        switch(current)
        {
        case SAF7730_INPUT_MUX_AUX:
            /* go to radio */
            new = SAF7730_INPUT_MUX_RADIO;
            break;
        case SAF7730_INPUT_MUX_RADIO:
            /* go to cd-rom */
            new = SAF7730_INPUT_MUX_CD;
            break;
        case SAF7730_INPUT_MUX_CD:
        default:
            /* go to aux mode */
            new = SAF7730_INPUT_MUX_AUX;
            break;
        }
    }
}

static void i2c_detection(bool enabled)
{
    if (enabled == true)
    {
        /* setup the i2c detection pin (SCL) */
        PCICR  |= _BV(PCIE1);   /* interrupt vector enable */
        PCMSK1 |= _BV(PCINT13);  /* mask the only required pin, SCL */
    }
    else
    {
        /* setup the i2c detection pin (SCL) */
        PCICR  &= ~_BV(PCIE1);   /* interrupt vector enable */
        PCMSK1 &= ~_BV(PCINT13);  /* mask the only required pin, SCL */
    }
}

static void i2c_init_override(void)
{
    /* two-wire init, also enables pull-ups */
    i2c_init();
    /* disable pull-ups for SDA and SCL: the bus is already provided with them */
    PORTC &= ~(_BV(PC4) | _BV(PC5));
}

static void i2c_deinit_override(void)
{
    /* Disable the TWI subsystem */
    TWCR = 0;
}

static void source_change(void)
{
    if (new != current)
    {
        DEBUG_EXEC( { uart_putstring("sending i2c command\r\n"); } );
        DEBUG_EXEC( { if (new == SAF7730_INPUT_MUX_AUX) uart_putstring("AUX\r\n");
                      if (new == SAF7730_INPUT_MUX_CD) uart_putstring("CDROM\r\n");
                      if (new == SAF7730_INPUT_MUX_RADIO) uart_putstring("RADIO\r\n");
                    } );
        i2c_detection(false);
        /* initialize the two-wire */
        i2c_init_override();
        /* perform the command */
        saf7730_input_mux_switch(new);
        /* deinitialize the two-wire interface */
        i2c_deinit_override();
        i2c_detection(true);
        current = new;
    }
}

static void status_led(bool enable)
{
    if (enable == true)
    {
        DDRB |= _BV(PINB5);
        PORTB |= _BV(PINB5);
    }
    else
    {
        DDRB &= ~_BV(PINB5);
        PORTB &= ~_BV(PINB5);
    }
}

static void cpu_sleep_periodic(void)
{
    DEBUG_EXEC({ uart_putstring("SLEEP\r\n"); _delay_ms(50); });

    /* notify sleep by led */
    for (int i = 0; i < 25; i++)
    {
        status_led((bool)(i & 0x1));
        _delay_ms(25);
    }

    /* go to power down mode, where PCINT will
     * wake the device up when i2c activity is detected
     * i.e. at car's radio ON
     * External Reset
     * Watchdog System Reset
     * Watchdog Interrupt
     * Brown-out Reset
     * 2-wire Serial Interface address match
     * External level interrupt on INT
     * Pin change interrupt
     * */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    cli();

    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();

    /* reset timers */
    TIMER_RESET(SOFT_TIMER_2);
    TIMER_RESET(SOFT_TIMER_3);
    /* prevent changing source if it was to
     * be applied before sleeping */
    new = current;

    /* notify sleep by led */
    for (int i = 0; i < 15; i++)
    {
        status_led((bool)(i & 0x1));
        _delay_ms(75);
    }

    sei();

    DEBUG_EXEC({ uart_putstring("RESUME\r\n"); });
}

int main(void)
{
    bool selector_released;

    /* disable all interrupts */
    cli();

    /* disable all the unnecessary peripherals..
     * ...we are on battery power! */
    power_adc_disable();
    power_spi_disable();
    power_timer1_disable();
    power_timer2_disable();

    /* Start 1000Hz system timer */
    OCR0A = F_CPU / 64 / 1000 - 1;
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS00) | _BV(CS01);
    TIMSK0 = _BV(OCIE0A);

    /* init timer */
    TIMER_RESET(SOFT_TIMER_0);
    TIMER_RESET(SOFT_TIMER_1);
    TIMER_RESET(SOFT_TIMER_2);
    TIMER_RESET(SOFT_TIMER_3);

    /* input selector button, input and pull-up */
    DDRD &= ~(_BV(PIND2));
    PORTD |= _BV(PIND2);

    /* setup the i2c detection pin (SCL) */
    i2c_detection(true);

    DEBUG_EXEC( { uart_init(); uart_putstring("DEBUG MODE\r\n"); } );

    /* enable all interrupts */
    sei();

    while (1)
    {

#ifdef DEBUG
        ON_TIMER_EXPIRED(1000, SOFT_TIMER_9, { uart_putstring("1SEC\r\n"); TIMER_RESET(SOFT_TIMER_5); } );

        if (debug_isr == true)
        {
            // i dont care about atomicity here, it's debug!
            itest = 0;
            debug_isr = false;
//            uart_putstring("receive some interrupts\r\n");
        }
#endif

        /* switch to ground means pressed */
        selector_released = bit_is_clear(PIND, PIND2);
        /* pass it to the keypad logic */
        keypad_set_input(BUTTON_SELECT, selector_released);

        /* this logic shall always be run periodically */
        keypad_periodic(timeout(10, SOFT_TIMER_1));

        if (keypad_clicked(BUTTON_SELECT) == KEY_CLICK)
        {
            /* button pressed */
            source_select_next();
        }

        /* if the i2c bus is not busy for at least 6 milliseconds, select
         * new source when required */
        ON_TIMER_EXPIRED(TX_AFTER_MS,    SOFT_TIMER_2, source_change());
        ON_TIMER_EXPIRED(SLEEP_AFTER_MS, SOFT_TIMER_3, cpu_sleep_periodic());

    }
    /* shall never get there, reset otherwise */
}
