# -*- coding: cp1252 -*-
# This code is part of DPAV4 project
# Copyright 2013 Telecom ParisTech
# Zakaria Najm
import sys
import os
import threading
import time
import logging
import math
import serial
import glob

class SmartcardSerial():
    
    def __init__(self):
        self.ser = None

    def __del__(self):
        self.close()
    
    def connect(self, serport):
        if self.ser == None:
            # Open serial port 
            self.ser = serial.Serial()
            self.ser.port     = serport
            self.ser.baudrate = 115200
            self.ser.timeout  = 3     # 3 second timeout
            self.ser.stopbits = serial.STOPBITS_ONE
            self.ser.parity = serial.PARITY_EVEN
            self.ser.open()
        
        self.reset()

    def reset(self):
        self.ser.flush()
        self.ser.setRTS(True)
        time.sleep(0.2)
        self.ser.setRTS(False)
        time.sleep(0.5)

        data = bytearray(self.ser.read(self.ser.inWaiting()))
        stratr = "ATR = "
        for p in data:
            stratr = stratr + "%c "%p

        print stratr
        return stratr
    
    def disconnect(self):
        self.close()
            
    def close(self):
        if self.ser != None:
            self.ser.close()
            self.ser = None
        
    def init(self):
        self.reset()

    def getATR(self):
        return self.stratr

    def APDUHeader(self, cla, ins, p1, p2, datalen):
        header = bytearray()
        header.append(datalen)
        header=str(header)
        header.append("/r/n")
        self.ser.flushInput()
        self.ser.write(header)
        
        return True

    def APDUSend(self, cla, ins, p1, p2, data):
        #self.APDUHeader(cla, ins, p1, p2, len(data))   
        self.ser.write(str(bytearray(data)))

        #For non-sasebow, get rid of echo
        self.ser.read(len(data))
        


        return True

    def APDURecv(self, cla, ins, p1, p2, datalen):
        self.APDUHeader(cla, ins, p1, p2, datalen)
        
        resp = self.ser.read(datalen + 2)

        if len(resp) < (datalen + 2):
            print "SASEBOW: APDUResp Data Error, response too short"
            return 0

        resp = bytearray(resp)
        
        return resp[0:datalen]        
                    
    def setModeEncrypt(self):
        return

    def setModeDecrypt(self):
        return

    def loadEncryptionKey(self, key):
        if key != None:
            self.APDUSend(0x80, 0x12, 0x00, 0x00, key)
      
    def loadInput(self, inputtext):
        self.input = inputtext

    def isDone(self):
        return True

    def readOutput(self):
        return self.APDURecv(0x80, 0xC0, 0x00, 0x00, 16)

    def enc(self,x,y,z):
        self.APDUSend(0x80,0xC0,0x00,0x00,x) #datai
        self.APDUSend(0x80,0xC0,0x00,0x00,y) #datai
        self.APDUSend(0x80,0xC0,0x00,0x00,z) #datai
        #time.sleep(.2)
        bytearray(self.ser.read(64))
        data = bytearray(self.ser.read(2))
        buf = "recu du C: \n\r"
        for p in data:
           buf = buf + "%c"%p
        print buf
        self.ser.flush()
        return buf
        
    def go(self):
        self.APDUSend(0x00, 0x04, 0x04, 0x00, self.input)


def main():
    test=SmartcardSerial()
    port=glob.glob('/dev/ttyACM*')[0]
    test.connect(port)
    data_tab = open("./VI_BP.txt", "r") 

    for i in range(8):
        x=data_tab.readline()[:-1]
        y=data_tab.readline()[:-1]
        z=data_tab.readline()[:-1]
	    print "x: " +x
	    print "y: " +y
	    print "z: " +z
	    test.enc(x,y,z)
   
if __name__ == "__main__":
    main() 
