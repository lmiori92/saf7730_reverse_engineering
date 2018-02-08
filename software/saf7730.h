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

#ifndef SAF7730_H_
#define SAF7730_H_

#define COMM_SAF7730_ADDR        (0x1C)  /* saf7730 i2c slave address */

#define SAF7730_INPUT_MUX_RADIO  (0x13)  /* using the internal DAC from the tuner chip */
#define SAF7730_INPUT_MUX_CD     (0x2B)  /* using the internal I2S DAC */
#define SAF7730_INPUT_MUX_AUX    (0x19)  /* actually, this enables the CD-IN analog inputs, but Opel CD30 is using these as AUX */

#endif /* SAF7730_H_ */
