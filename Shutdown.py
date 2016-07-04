import re
import serial
import time
import subprocess
from piwho import recognition

WORDS = ['Shutdown']

def shutdown():
   connect = serial.Serial('/dev/ttyACM0',9600)
   time.sleep(0.5)
   connect.write('|A|S|300 ')
   time.sleep(0.5)

def centerpos():
	connect = serial.Serial('/dev/ttyACM0',9600)
	time.sleep(0.5)
	connect.write('|A|B|300 ')
	time.sleep(0.5)

def reset_arduino():
        conenct = serial.Serial('/dev/ttyACM0',9600)
        time.sleep(0.5)
        connect.write('|A|R|300 ')
	time.sleep(0.5)

def authorize():
	recog = recognition.SpeakerRecognizer('/home/pi/jasper/recordings/')
        recog.set_feature_option('-lpc -cheb')
	name  = recog.identify_speaker()
	if name == 'Aditya':
	     return True
        else:
             return False


def handle(text,mic,profile):
	mic.say('Shutdown requires a voice authentication ')
        mic.say('record your voice after high beep.')
	mic.activeListen()
	if authorize():
             mic.say('Authetication successfull')
             mic.say('This will reset all peripherals')
             mic.say('Should I continue?')
             reply = mic.activeListen()
             if 'YES' in reply:
		mic.say('centering legs')
		centerpos()
                mic.say('Resetting Arduino')
		reset_arduino()
                mic.say('Shutting down')
		shutdown()
		subprocess.call("sudo shutdown -h now",shell=True)
	     elif 'NO' in reply:
		mic.say('Shutdown aborted')
	else:
	    mic.say('Authentication failed')
 	    mic.say('Shutdown aborted')

def isValid(text):
	return bool(re.search(r'\bshutdown\b',text,re.IGNORECASE))

