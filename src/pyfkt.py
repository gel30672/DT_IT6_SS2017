import serial

def multiply(a,b):
    print "Will compute", a, "times", b
    c = 0
    for i in range(0, a):
        c = c + b
    return c

def add(a, b):
    c = 0
    c = a+b
    return c

def read():
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600)

    line = ser.readline()

    #Get Data

    #Cut usless Stuff xD
    AnchorData = line[6:41]
    AmountOfChars = len(AnchorData)

    #Get Anchor Data
    Index = 0
    firstIndex = 0
    AnchorNr = 0
    Anchor=['x','x','x','x'] # 4 Anchors
    print "AnchorData:" + AnchorData

    while Index < AmountOfChars:
        if AnchorData[Index] == ' ':
            Anchor[AnchorNr] = AnchorData[firstIndex:Index]
            firstIndex = Index+1
            AnchorNr = AnchorNr+1
        elif Index+1 == AmountOfChars:
            Anchor[AnchorNr] = AnchorData[firstIndex:Index+1] #Index+1 last Index

        Index = Index+1

    #Print Anchor Data
    #print "Anchor0:" + Anchor[0]
    #print "Anchor1:" + Anchor[1]
    #print "Anchor2:" + Anchor[2]
    #print "Anchor3:" + Anchor[3]
    ser.close()
    return 0
