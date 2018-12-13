
#!/usr/bin/env python
import time
import serial
import os
# /!\ The "serial" package is not necessarily installed on Windows :(

# TODO: configure the adequate device, depending on the target:
#dev = '/dev/ttyUSB1' # With an FTDI USB <=> RS232 wire
#dev = '/dev/ttyS0' # With a serial wire
dev =  '/dev/ttyACM1'
#dev = 'COM54'         # On Windows

ser = serial.Serial(dev,115200,xonxoff=0, rtscts=0,bytesize=8, parity='N', stopbits=1, timeout=50)
ser.isOpen()
ser.flushOutput()
ser.flushInput()
#ser.close()
# ser.isOpen() pour tester s'il est vraiment ouvert
print( "Hello: test!" )

# register mapping
#import regs

def write (data):
        ser.write(chr( data ))

def read ():
        return ord(ser.read())

def byte (val, index):
        return ((val>>(8*index)) & 0xff)

def read32():
        x=0
        x   =  read() <<24 | x
        x   =  read() <<16 | x
        x   =  read() <<8  | x
        x   =  read()      | x
        return ord(x)
        

#flushInput()
#flushOutput()


#write(byte(Key, 0), regs.REG_KEY_00)
#write(byte(Key, 1), regs.REG_KEY_01)
#write(byte(Key, 2), regs.REG_KEY_02)
#write(byte(Key, 3), regs.REG_KEY_03)
#write(byte(Key, 4), regs.REG_KEY_04)
#write(byte(Key, 5), regs.REG_KEY_05)
#write(byte(Key, 6), regs.REG_KEY_06)
#write(byte(Key, 7), regs.REG_KEY_07)
#write(byte(Key, 8), regs.REG_KEY_08)
#write(byte(Key, 9), regs.REG_KEY_09)
#write(byte(Key,10), regs.REG_KEY_10)
#write(byte(Key,11), regs.REG_KEY_11)

cText = 0
ct0    = 0
ct1    = 0
ct2    = 0
ct3    = 0
ct4    = 0
ct5    = 0
ct6    = 0
ct7    = 0
ct8    = 0
ct9    = 0
ct10    = 0
ct11   = 0
ct12    = 0
ct13    = 0
ct14    = 0
ct15    = 0
cc0=0

i=0l

#initiate handshake
write(0x45)
# write pt and key

pText = 0x00000000000000000000000000000000
key =   0x00000000000000000000000000000000
#while i < 16:
        #write(byte(pText, i))
        #i=i+1
        #print( " pt %s is %s "%( str( i), hex(byte(pText, i))))
#time.sleep(1)        
#i=0l
#while i < 16:
        #write(byte(key, i))
#        i=i+1
        #print( " key %s is %s "%( str( i), hex(byte(key, i))))
#time.sleep(1)

#config skey
        #0x01  = default  study  key 000000000000000000000000
        #0x02  =  default attack key
        #0x03  =  PC sends the secretkey to Discovery board
#write(0x01)
#time.sleep(2)
#write PT mode
        #0x01 = pt is generated in STM Micro controller
        #0x02 = pt is sent from PC -> to Discovery Board
#write(0x01)

#write CT and timing data mode
        #0x01 = Only timing data is requested
        #0x02 = Timing data and plaintedt is requested
        #0x02 =
#write(0x02)

#time.sleep(1)
f = open('workfile-DSx03-cou-var.txt', 'w')

#for i in range(long(400000000)):
while i < 4000000000:
        i = i + 1
        # wait for data
        while ser.inWaiting() < 20:
                time.sleep(.01)
                #write(0x46)
                 
        cText   =  0
        cText   =  read() <<24 | cText
        cText   =  read() <<16 | cText
        cText   =  read() <<8  | cText
        cText   =  read()      | cText
        
        ct0 =  read()
        ct1 =  read()
        ct2 =  read()
        ct3 =  read()
        
        ct4 =  read()
        ct5 =  read()
        ct6 =  read()
        ct7 =  read()

        ct8 =  read()
        ct9 =  read()
        ct10 =  read()
        ct11 =  read()

        ct12 =  read()
        ct13 =  read()
        ct14 =  read()
        ct15 =  read()
        
        #print( " --> %s %s %s %s %s %s %s %s %s %s"%( str( cText), hex(ct0) , hex(ct1) , hex(ct2), hex(ct3), str(cc0) , str(cc1) , str(cc2) , str(cc3) , str(cc4)   ) )
        print("%s "%(str(cText)))
        #f.write(str(ct0) + ' '+ str(ct1) +' '+  str(ct2) +' '+  str(ct3) +' '+  str(ct4) +' '+  str(ct5) +' '+  str(ct6) +' '+  str(ct7) +' '+  str(ct8)  +' '+ str(ct9) +' '+ str(ct10) +' '+str(ct11) +' '+ str(ct12) +' '+ str(ct13) +' '+  str(ct14) +' '+  str(ct15) + '  '+ str(cText)+'\n')
        #f.write(str(cText)+' '+ str(ct0) + ' '+ str(ct1) +' '+  str(ct2) +' '+  str(ct3) +' '+  str(ct4) +' '+  str(ct5) +' '+  str(ct6) +' '+  str(ct7) +' '+  str(ct8)  +' '+ str(ct9) +' '+ str(ct10) +' '+str(ct11) +' '+ str(ct12) +' '+ str(ct13) +' '+  str(ct14) +' '+  str(ct15) + '\n')
        f.write(str(cText)+' '+ hex(ct0) + ' '+ hex(ct1) +' '+  hex(ct2) +' '+  hex(ct3) +' '+  hex(ct4) +' '+  hex(ct5) +' '+  hex(ct6) +' '+  hex(ct7) +' '+  hex(ct8)  +' '+ hex(ct9) +' '+ hex(ct10) +' '+hex(ct11) +' '+ hex(ct12) +' '+ hex(ct13) +' '+  hex(ct14) +' '+  hex(ct15) + '\n')
        #f.write(str(cText)+'\n')
        #time.sleep(0.002)
        os.fsync(f)
ser.close()
