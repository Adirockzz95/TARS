# -*- coding: utf-8-*-
import random
import re
import time
import serial
from client import jasperpath

WORDS = ["JOKE", "KNOCK KNOCK"]

def handle(text, mic, profile):
    """
        Responds to user-input, typically speech text, by telling a joke.

        Arguments:
        text -- user-input, typically transcribed speech
        mic -- used to interact with the user (for both input and output)
        profile -- contains information related to the user (e.g., phone
                   number)
    """
    led = serial.Serial('/dev/ttyACM0',9600)
    time.sleep(0.5)
    led.write('|A|J|3500 ')
    time.sleep(0.3)
    mic.say(' A robot walks into a bar.')
    mic.say('What can i get you?  the bartender asks')
    mic.say('I need something to loosen up,')
    mic.say('the robot replies.')
    mic.say('So the bartender serves him a screwdriver.')
    mic.say(' ha ha ha')


def isValid(text):
    """
        Returns True if the input is related to jokes/humor.

        Arguments:
        text -- user-input, typically transcribed speech
    """
    return bool(re.search(r'\bjoke\b', text, re.IGNORECASE))
