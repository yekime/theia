import speech_recognition as sr
from gtts import gTTS
import os, sys
import time
import playsound


# import pyttsx3

# engine = pyttsx3.init("sapi5")
# voices = engine.getProperty("voices")  # fetching different voices from the system
# engine.setProperty("voice", voices[1].id)  # setting voice properties
# engine.setProperty("rate", 130)  # sets speed of speech

import time, os, sys, contextlib


@contextlib.contextmanager
def suppress():
    devnull = os.open(os.devnull, os.O_WRONLY)
    old_stderr = os.dup(2)
    sys.stderr.flush()
    os.dup2(devnull, 2)
    os.close(devnull)
    try:
        yield
    finally:
        os.dup2(old_stderr, 2)
        os.close(old_stderr)


def speak(text):
    # engine.say(text)
    # engine.runAndWait()

    tts = gTTS(text=text, lang="en")
    filename = "voice.mp3"
    tts.save(filename)
    playsound.playsound(filename)


def listen():
    with suppress():
        r = sr.Recognizer()
        with sr.Microphone() as source:
            r.adjust_for_ambient_noise(source)
            audio = r.listen(source)
            said = ""

            try:
                said = r.recognize_google(audio)
            except Exception as e:
                pass

        return said


# speak("activated")
def get_mikes():
    with suppress():
        l = sr.Microphone.list_microphone_names()
        print("printing microphone names:")
        print(l)


WAKE = "hey thea"

# def contains_wake(command):
# get_mikes()

while True:
    print("Listening")
    command = listen().lower()
    if command.count(WAKE) > 0:
        speak("How can I help you?")
        command = listen()
