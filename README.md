# saf7730_reverse_engineering
SAF7730 reverse engineering material and data for analyzing how to toggle input source programmatically on a Opel CD30 Grundig Delphi variant.

# Contributions

Contributions are clearly welcome! Please create a pull request with your additional information or software you have. If you are not experienced with git(hub), then please also drop an e-mail. 

# Goal
Understand the communication protocol since there is no available public documentation.

My Opel CD30 has no AUXiliary input. Eletronic connection from outside is missing as well as the programming required. Interestingly, early Blaupunkt's were able to, but to possibly save some cents they have avoided including the required electronics by default. What a shame... A mono-only phone-in line is available and freely switchable by a signal pin that, when grounded, switches the radio receiver to "External In" mode.

The ultimate goal is to exploit the phone-in switching and finally externally switching the input analog mux to AUX (which provides a stereo input!)

# Possible issues

Since the phone line is mono only, perhaps the radio software cannot reliably work with it (e.g. volume adjust) and therefore my programming shall cover some more registers other than the source mux selector.

# Target solution

A AVR microcontroller that writes some registers to the device once the phone-in line has been selected.

From the sampled i2c communication it appears the bus'MCU is quite busy sending and receiving data but there are times where the bus is free for more than 50ms and that's where we will be going to write our super-imposed register content.

We don't know if i2c multi-master is supported by the radio's MCU and therefore the safest way is to just wait at least some milliseconds (to be determined from the sampled communication) after the last communication before writing our data.

# Supporting material

Code that uses a similar variant (SAF7741HV) which helped a lot in confirming some findings.
Please note that some registers addresses seems to be slightly different, though.

https://github.com/huangjionghui/8803-project/tree/master/flyProject/driver/FlyAudio

https://github.com/huangjionghui/mtk4Overseas/tree/master/driver-code/flydrv/FLY7741AUDIO

http://www.crystalradio.cn/thread-578039-1-1.html

https://www.nxp.com/docs/en/data-sheet/SAA7724H.pdf   (some registers that are documented have names that are comparable or equal to the ones in this datasheet)

You are invited to explore the code base, it contains several (possible) references to the datasheet's register names and the logic as well.

# Pinout SAF7730HV

Pinout can be gathered from service manuals for other devices and from some product datasheets.

http://magnitola.org/attachments/clarion/555822d1397136565-Magnitola-Avtozvuk-clarion_dxz955mc.pdf

Clarion VRX765VD

# I2C bus sniffing samples and decoded reports

The repository contains a bunch of sampled "situations" e.g. switching from a source to another, raising or lowering the volume and power on/off sequences.

All the sampled situations are done by using sigrok (actually, it's "pulseview" gui interface) and a Saleae Logic clone (I don't remeber on top of my head if it contains the open firmware for sigrok or the original cloned one...).

.sr  : the sigrok session (10 Meg samples @ 1MHz)
.csv : the csv file produced with a script I wrote in python to analyze data more easily and efficiently using LibreOffice Calc
.ods : the final Open Document format containing the csv and some more protocol decoded data

# Scripts

i2c-sigrok-to-csv.py: running the i2c signal decoder on the .sr yields data that cannot be really analyed clearly in a spreadsheet and therefore I wrote a quick script to perform a nicer representation, where every i2c message is on one row.

dsp_csv_data_to_bin.py: the captured power on sequence reveals that at least 2 parts of a firmware is loaded on the device. They could be also "only" 4Kb worth of registers, but I highly doubt it!

# The protocol

The DSP appears to be a 24-bit unit since register addressing and reset vectors are all 24-bits long.

A write is composed by the canonical i2c ADDRESS + R/W BIT, followed by a 24-bit address, followed by the data that shall be written to the registers and adjacent ones ( the length of a register is not intuitively 24-bits...it seems that it might vary? Or perhaps the write just does not replace the content of the missing bit).

To initiate a read operation, a write operation with the requested address and no data is performed (i.e. like a normal write but with 0 data bytes)

A read is composed by the canonical i2c ADDRESS + R/W BIT, followed by the data that is read at the previously pointed location. The master decides how many bytes to receive (?).
