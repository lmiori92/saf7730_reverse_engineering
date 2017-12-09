import csv

inputdata = ""

# input file created wiht
# sigrok-cli -P i2c:scl=1:sda=0:address_format=shifted -i i2c_cdrom_to_phone.sr -A i2c=address-read:address-write:data-read:data-write
# from a captured session made with PulseView (using sigrok)

#Write
#Address write: 1C
#Data write: 01
#Data write: 00
#Data write: CB
#Read
#Address read: 1C
#Data read: 3F
#Data read: 6D
#Data read: 25
#Write

with open("test", "rb") as inputfile:
    inputdata = inputfile.read()

with open('test.csv', 'wb') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)

    buffer = ["Type", "Address"] + [ "Data%i"%i for i in range(0,20)]

    for line in inputdata.splitlines():
        if line.startswith("Write"):
            spamwriter.writerow(buffer)
            buffer = ["Write"]
        elif line.startswith("Read"):
            spamwriter.writerow(buffer)
            buffer = ["Read"]
        elif line.startswith("Address write"):
            buffer += [line.split(": ")[1]]
        elif line.startswith("Address read"):
            buffer += [line.split(": ")[1]]
        elif line.startswith("Data read"):
            buffer += [line.split(": ")[1]]
        elif line.startswith("Data write"):
            buffer += [line.split(": ")[1]]

    spamwriter.writerow
