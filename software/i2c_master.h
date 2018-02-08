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

#ifndef __I2C_I2C_MASTER__
#define __I2C_I2C_MASTER__

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

/* TODO Currenty the code is not well platform independent regarding the status codes ...*/

/** Defines the target (wanted) i2c clock [Hz] */
#define TWI_CLOCK_TARGET    200000U

/** Calculate the TWBR (TWI Bit rate) register value */
/* (CPU / SCL - 16) / ( 2 * prescaler ) */
#ifdef F_CPU
#define TWI_TWBR            (((F_CPU / TWI_CLOCK_TARGET) - 16U) / ( 2U * 1U ))
#else
/* dummy define */
#define TWI_TWBR            (0U)
#endif

/** us units. WARNING: no more than 255! */
#define I2C_MASTER_TIMEOUT  200U

#define TWI_READ_BIT  0       /**< Bit position for R/W bit in "address byte" */
#define TWI_ADR_BITS  1       /**< Bit position for LSB of the slave address bits in the init byte */

/* General TWI Master status codes */
#define TWI_START                  0x08  /**< START has been transmitted */
#define TWI_REP_START              0x10  /**< Repeated START has been transmitted */
#define TWI_ARB_LOST               0x38  /**< Arbitration lost */

/* TWI Master Transmitter status codes */
#define TWI_MTX_ADR_ACK            0x18  /**< SLA+W has been transmitted and ACK received */
#define TWI_MTX_ADR_NACK           0x20  /**< SLA+W has been transmitted and NACK received */
#define TWI_MTX_DATA_ACK           0x28  /**< Data byte has been transmitted and ACK received */
#define TWI_MTX_DATA_NACK          0x30  /**< Data byte has been transmitted and NACK received */

/* TWI Master Receiver status codes */
#define TWI_MRX_ADR_ACK            0x40  /**< SLA+R has been transmitted and ACK received */
#define TWI_MRX_ADR_NACK           0x48  /**< SLA+R has been transmitted and NACK received */
#define TWI_MRX_DATA_ACK           0x50  /**< Data byte has been received and ACK transmitted */
#define TWI_MRX_DATA_NACK          0x58  /**< Data byte has been received and NACK transmitted */

/* TWI Slave Transmitter status codes */
#define TWI_STX_ADR_ACK            0xA8  /**< Own SLA+R has been received; ACK has been returned */
#define TWI_STX_ADR_ACK_M_ARB_LOST 0xB0  /**< Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned */
#define TWI_STX_DATA_ACK           0xB8  /**< Data byte in TWDR has been transmitted; ACK has been received */
#define TWI_STX_DATA_NACK          0xC0  /**< Data byte in TWDR has been transmitted; NOT ACK has been received */
#define TWI_STX_DATA_ACK_LAST_BYTE 0xC8  /**< Last data byte in TWDR has been transmitted (TWEA = �0�); ACK has been received */

/* TWI Slave Receiver status codes */
#define TWI_SRX_ADR_ACK            0x60  /**< Own SLA+W has been received ACK has been returned  */
#define TWI_SRX_ADR_ACK_M_ARB_LOST 0x68  /**< Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned  */
#define TWI_SRX_GEN_ACK            0x70  /**< General call address has been received; ACK has been returned */
#define TWI_SRX_GEN_ACK_M_ARB_LOST 0x78  /**< Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned */
#define TWI_SRX_ADR_DATA_ACK       0x80  /**< Previously addressed with own SLA+W; data has been received; ACK has been returned */
#define TWI_SRX_ADR_DATA_NACK      0x88  /**< Previously addressed with own SLA+W; data has been received; NOT ACK has been returned */
#define TWI_SRX_GEN_DATA_ACK       0x90  /**< Previously addressed with general call; data has been received; ACK has been returned */
#define TWI_SRX_GEN_DATA_NACK      0x98  /**< Previously addressed with general call; data has been received; NOT ACK has been returned */
#define TWI_SRX_STOP_RESTART       0xA0  /**< A STOP condition or repeated START condition has been received while still addressed as Slave */

/* TWI Miscellaneous status codes */
#define TWI_NO_STATE               0xF8  /**< No relevant state information available; TWINT = 0 */
#define TWI_TIMEOUT                0x01  /**< Bus error due to a timeout */
#define TWI_BUS_ERROR              0x00  /**< Bus error due to an illegal START or STOP condition */

extern uint16_t error_counter;

void    i2c_init(void);
uint8_t i2c_busy(void);
uint8_t i2c_get_state_info(void);

void    i2c_transfer_start(void);
void    i2c_transfer_set_data(uint8_t *data, uint8_t len);
uint8_t i2c_transfer_successful(void);

#endif      /* __I2C_I2C_MASTER__ */
