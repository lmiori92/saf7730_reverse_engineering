

of = open("dsp_firmware_opel_cd30_2.bin","wb")

with open("dsp_firmware_opel_cd30_2.csv","rb") as dsp_ascii:
    
    content = dsp_ascii.read()
    lines = content.splitlines()
    
    for line in lines:
        for b in line.split(","):
            if b != '':
                of.write(chr(int(b,16)))


of.close()