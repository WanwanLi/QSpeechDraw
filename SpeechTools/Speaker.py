import sys;
import win32com.client as win32;

speaker=win32.Dispatch("SAPI.SpVoice");
text="";
for i in range(1, len(sys.argv)):
	text=text+" "+sys.argv[i];
speaker.Speak(text);

