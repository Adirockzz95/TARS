import re
import serial
import time

WORDS=['WALK']

def walk():
    connect = serial.Serial('/dev/ttyACM0',9600)
    time.sleep(0.5)
    connect.write('|A|W|350 ')

def handle(text,mic,profile):
    mic.say("roger that")
    walk()

def isValid(text):
     return bool(re.search(r'\walk\b',text,re.IGNORECASE))
