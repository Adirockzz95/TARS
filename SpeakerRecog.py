import re

from piwho import recognition
from lium import genderdetect as gd

WORDS = ["HELLO","TARS"]

def recog():
	spkr = recognition.SpeakerRecognizer('/home/pi/jasper/recordings/')
	spkr.set_feature_option('-lpc')
	name = spkr.identify_speaker()
        return name 

def handle(text, mic, profile):
	spname = recog()
	
	if 'unknown' not in spname:
		mic.say("Hello "+ spname)
	else:
		filename = recog.get_recently_added_file()
		gender = gd.identify_gender(filename)
		if 'M' in gender:
			mic.say('Hello Sir')
		elif 'F' in gender:
			mic.say('Hello Madam')
	
def isValid(text):
	return bool(re.search(r'\bhello\b',text,re.IGNORECASE))
