import time;
import win32com.client as win32;
import speech_recognition as SpeechRecognition;

speaker=win32.Dispatch("SAPI.SpVoice");
recognizer=SpeechRecognition.Recognizer();

def speak(fileName):
	file=open(fileName,'r');
	for text in file:
		speaker.Speak(text);
	file.close();

def write(fileName, text):
	file=open(fileName,'w');
	file.write(text);
	file.close();

def listen(timeout):
	with SpeechRecognition.Microphone() as microphone:
		recognizer.adjust_for_ambient_noise(microphone, duration=1);
		speaker.Speak("Please speak");
		try:
			audio=recognizer.listen(microphone, timeout);
			text=recognizer.recognize_google(audio);
			if(text.find("help")!=-1): speak("help.txt");
			else: 
				write("../audio.txt", text); 
				return;
		except SpeechRecognition.UnknownValueError:
			speaker.Speak("Could not understand");
		except SpeechRecognition.WaitTimeoutError:
			speaker.Speak("Could not hear anything");
		except SpeechRecognition.RequestError:
			speaker.Speak("Could not request results");
	write("../audio.txt", "null");

listen(2);
