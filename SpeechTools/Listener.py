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

def listen(timeout, timeLimit):
	with SpeechRecognition.Microphone() as microphone:
		recognizer.adjust_for_ambient_noise(microphone, duration=1);
		speaker.Speak("Speak");
		try:
			audio=recognizer.listen(microphone, timeout, timeLimit);
			text=recognizer.recognize_google(audio); 
			if(text.find("help")!=-1): speak("help.txt");
			else: 
				write("../audio.txt", text); 
				return;
		except SpeechRecognition.UnknownValueError:
			write("../audio.txt", "null");
		except SpeechRecognition.WaitTimeoutError:
			write("../audio.txt", "null");
		except SpeechRecognition.RequestError:
			write("../audio.txt", "null");
listen(2, 3);
