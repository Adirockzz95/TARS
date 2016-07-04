import re
import serial
import time
WORDS = ['RESET']

def reset():
    connect = serial.Serial('/dev/ttyACM0',9600)
    time.sleep(0.5)
    connect.write('|A|R|1000 ')
    time.sleep(0.5)

def handle(text,mic,profile):
    mic.say('Do you want to reset the Arduino?')
    reply = mic.activeListen()
    if reply=='YES':
       Reset()
       mic.say('roger that')
    elif reply=='NO':
       mic.say('Arduino reset cancelled')

def isValid(text):
     return bool(re.search(r'\breset\b',text,re.IGNORECASE))

