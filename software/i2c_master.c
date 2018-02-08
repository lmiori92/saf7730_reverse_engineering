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

/* Module */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "i2c_master.h"

/* Standard Library */
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/** Timeout counter for diagnostics */
uint16_t error_counter = 0;
/** Data (message) buffer [bytes] */
static uint8_t *i2c_xfer_buffer;
/** Size to be transfered (rx or tx) [bytes] */
static uint8_t i2c_xfer_size;
/** i2c state (last value read from TWSR) */
static uint8_t i2c_state = TWI_NO_STATE;
/** i2c last operation successful */
static bool i2c_successful_operation;

/**
 * Initialize the i2c subsystem.
 */
void i2c_init(void)
{
    /* Set the birate register, which is computed at compile time */
    TWBR = TWI_TWBR;

    /* Enable the TWI subsystem (initializing the pins) */
    TWCR = (1 << TWEN);

    /* Set pull-ups on SDA and SCL pins */
    PORTC |= _BV(PC4) | _BV(PC5);

    /* buffer is initially "null" */
    i2c_xfer_buffer = NULL;
}

/**
 * Get the i2c subsystem busy flag
 *
 * @return      0: not busy; else: busy
 */
uint8_t i2c_busy(void)
{
    return (TWCR & (1 << TWIE));
}

/**
 * Wait for the i2c interface to be ready (= not busy).
 * A timeout can occur and if so the i2c state will be
 * set to a TIMEOUT error and the interface reset as well.
 */
static inline void i2c_wait_or_timeout(void)
{
    uint8_t i = 0;
    do
    {
        i++;
        _delay_us(5);
        if (i > I2C_MASTER_TIMEOUT)
        {
            /* Set the error ... */
            i2c_state = TWI_TIMEOUT;
            /* ... and reset the TWI interface */
            TWCR = (1 << TWEN) |       // Enable TWI-interface and release TWI pins
                    (0 << TWIE) | (0 << TWINT) |               // Disable Interrupt
                    (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests
                    (0 << TWWC);                                 //
        }
    } while (TWCR & (1 << TWIE));
}

/****************************************************************************
 Call this function to fetch the state information of the previous operation. The function will hold execution (loop)
 until the TWI_ISR has completed with the previous operation. If there was an error, then the function
 will return the TWI State code.
 ****************************************************************************/
/**
 * Get the I2C subsystem status register. This is used to determine at application
 * level the cause of an error. If the transfer has not yet been completed, then
 * the function will wait for it to be so.
 *
 * @return      status register
 */
uint8_t i2c_get_state_info(void)
{
    return i2c_state;
}

/**
 * Set the DATA and LENGTH transfer properties. They can only be sent
 * atomically i.e. when no transfer is currently ongoing. If one is,
 * then the function will wait till completion.
 *
 * @param data      pointer to the data to be transfered from / to. The pointer shall point to
 *                  a static object OR to a dynamic object (stack, malloc, etc) if and only if
 *                  the object stays valid until the operation has been completed. Undefined behavior
 *                  happens otherwise. Additionally, it shall remain valid if multiple calls to
 *                  i2c_transfer_start are done in different execution contexts.
 * @param len       length of the data to be transmitted or to be expected
 */
void i2c_transfer_set_data(uint8_t *data, uint8_t len)
{
    if (data != NULL)
    {

        /* Wait until TWI is ready for next transmission */
        i2c_wait_or_timeout();

        /* set the data pointer */
        i2c_xfer_buffer = data;
        /* set the transmit length */
        i2c_xfer_size = len;

    }
    else
    {
        /* NULL pointer, screw you! */
    }
}

/**
 *
 * Start a transmission operation on the i2c bus.
 * DATA and LENGTH will determine the SLA (ADDR+R/W) and
 * additional data depending on the SLAVE device.
 * DATA and LENGTH are set internally by the function i2c_set_data.
 * If never set, then no transfer is possible.
 * If set at least once, then the transfer occurs on such data.
 * This mechanism allows a more efficient repeated transfer if the data
 * pointer, its content and/or its length remains constant.
 *
 */
void i2c_transfer_start(void)
{
    if (i2c_xfer_buffer != NULL)
    {

        /* Wait until TWI is ready for next transmission */
        i2c_wait_or_timeout();

        /* initially, the operation is failed by definition */
        i2c_successful_operation = false;
        /* initially, unknown state */
        i2c_state = TWI_NO_STATE;

        TWCR = (1 << TWEN) |                             // TWI Interface enabled.
                (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag.
                (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a START condition.
                (0 << TWWC);                             //

    }
    else
    {
        /* NULL pointer, screw you! */
    }
}

/**
 * Get the successful status of a tranfer. If a transfer is ongoing,
 * then the function will wait for it to complete.
 * @return  0: transfer not successful; 1: transfer successful
 */
uint8_t i2c_transfer_successful(void)
{
    /* Wait until TWI is ready for next transmission */
    i2c_wait_or_timeout();

    if (i2c_successful_operation == false)             error_counter++;

    return i2c_successful_operation;
}

/**
 * Interrupt service routine for the TWI interrupt flag.
 * It is called when an operation on the i2c bus has been performed.
 */
ISR(TWI_vect)
{
    static uint8_t i2c_buf_pos;

    /* mask the register to be compatible also with other prescaler values */
    switch (TWSR & 0xF8U)
    {
    case TWI_START:
        /* START has been transmitted */

    case TWI_REP_START:
        /* Repeated START has been transmitted */

        i2c_buf_pos = 0;
        /* no break : this is intended and not a bug */

    case TWI_MTX_ADR_ACK:
        /* SLA+W has been transmitted and ACK received */

    case TWI_MTX_DATA_ACK:
        /* Data byte has been transmitted and ACK received */

        if (i2c_buf_pos < i2c_xfer_size)
        {
            /* transfer is has not yet been completed */
            TWDR = i2c_xfer_buffer[i2c_buf_pos++];

            TWCR = (1 << TWEN) |                       // TWI Interface enabled
                    (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag to send byte
                    (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) |           //
                    (0 << TWWC);                                 //
        }
        else                    // Send STOP after last byte
        {

            /* operation is successfully completed */
            i2c_successful_operation = true;

            TWCR = (1 << TWEN) |                       // TWI Interface enabled
                    (0 << TWIE) | (1 << TWINT) | // Disable TWI Interrupt and clear the flag
                    (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) | // Initiate a STOP condition.
                    (0 << TWWC);                                 //
        }

        break;

    case TWI_MRX_DATA_ACK:
        /* Data byte has been received and ACK transmitted */

        i2c_xfer_buffer[i2c_buf_pos++] = TWDR;
        /* no break : this is intended and not a bug */

    case TWI_MRX_ADR_ACK:

        /* SLA+R has been transmitted and ACK received */

        if (i2c_buf_pos < (i2c_xfer_size - 1))  // Detect the last byte to NACK it.
        {
            TWCR = (1 << TWEN) |                       // TWI Interface enabled
                    (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag to read next byte
                    (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // Send ACK after reception
                    (0 << TWWC);                                 //
        }
        else                    // Send NACK after next reception
        {
            TWCR = (1 << TWEN) |                       // TWI Interface enabled
                    (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag to read next byte
                    (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // Send NACK after reception
                    (0 << TWWC);                                 //
        }

        break;

    case TWI_MRX_DATA_NACK:

        /* Data byte has been received and NACK transmitted */

        i2c_xfer_buffer[i2c_buf_pos] = TWDR;
        i2c_successful_operation = true;

        TWCR = (1 << TWEN) |                           // TWI Interface enabled
                (0 << TWIE) | (1 << TWINT) | // Disable TWI Interrupt and clear the flag
                (0 << TWEA) | (0 << TWSTA) | (1 << TWSTO) | // Initiate a STOP condition.
                (0 << TWWC);                                 //

        break;
    case TWI_ARB_LOST:

        /* Arbitration lost */

        TWCR = (1 << TWEN) |                           // TWI Interface enabled
                (1 << TWIE) | (1 << TWINT) | // Enable TWI Interrupt and clear the flag
                (0 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | // Initiate a (RE)START condition.
                (0 << TWWC);                                 //

        break;

    case TWI_MTX_ADR_NACK:
        /* SLA+W has been transmitted and NACK received */
    case TWI_MRX_ADR_NACK:
        /* SLA+R has been transmitted and NACK received */
    case TWI_MTX_DATA_NACK:
        /* Data byte has been transmitted and NACK received */
    case TWI_BUS_ERROR:
        /* Bus error due to an illegal START or STOP condition */
    default:

        /* save the state register */
        i2c_state = TWSR;
        /* ... and reset the TWI interface */
        TWCR = (1 << TWEN) |       // Enable TWI-interface and release TWI pins
                (0 << TWIE) | (0 << TWINT) |               // Disable Interrupt
                (0 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | // No Signal requests
                (0 << TWWC);                                 //

    }
}
