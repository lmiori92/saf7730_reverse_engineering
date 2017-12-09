# saf7730_reverse_engineering
SAF7730 reverse engineering material and data for analyzing how to toggle input source programmatically on a Opel CD30 Grundig Delphi variant.

# Goal
Understand the communication protocol since there is no available public documentation.

My Opel CD30 has no AUXiliary input. Eletronic connection from outside is missing as well as the programming required. Interestingly, early Blaupunkt's were able to, but to possibly save some cents they have avoided including the required electronics by default. What a shame... A mono-only phone-in line is available and freely switchable by a signal pin that, when grounded, switches the radio receiver to "External In" mode.

The ultimate goal is to exploit the phone-in switching and finally externally switching the input analog mux to AUX (which provides a stereo input!)

# Possible issues

Since the phone line is mono only, perhaps the radio software cannot reliably work with it (e.g. volume adjust) and therefore my programming shall cover some more registers other than the source mux selector.

# Supporting material

Code that uses a similar variant (SAF7741HV) which helped a lot in confirming some findings.
Please note that some registers addresses seems to be slightly different, though.

https://github.com/huangjionghui/8803-project/tree/master/flyProject/driver/FlyAudio

https://github.com/huangjionghui/mtk4Overseas/tree/master/driver-code/flydrv/FLY7741AUDIO

You are invited to explore the code base, it contains several (possible) references to the datasheet's register names and the logic as well.

# Pinout SAF7730HV

Pinout can be gathered from service manuals for other devices and from some product datasheets.

http://magnitola.org/attachments/clarion/555822d1397136565-Magnitola-Avtozvuk-clarion_dxz955mc.pdf

Clarion VRX765VD

