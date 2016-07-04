import re
import serial
import time

WORDS=['RELAX','DEFAULT']

def relax():
    connect = serial.Serial('/dev/ttyACM0',9600)
    connect.write('|A|D|300 ')
    time.sleep(0.5)
    
def handle(text,mic,profile):
    mic.say("roger that")
    mic.say("going into default position")
    relax()
    mic.say("Done")

def isValid(text):
     return bool(re.search(r'\brelax\b',text,re.IGNORECASE))
