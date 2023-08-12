import speech_recognition as sr
from gtts import gTTS
import playsound
import os, sys, contextlib
import colorutils
import chime
import requests
from enum import Enum

RUN_SERVER = True
WAKE = ["hey thea", "hey there", "thea"]
URL = "http://192.168.1.71/"
IGNORE_WORDS = [
    "set",
    "it",
    "to",
    "get",
    "give",
    "me",
    "i",
    "want",
    "send",
    "vibe",
    "color",
    "brightness",
    "thea",
]
chime.theme("material")

mode = "DEFAULT"

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
    if mode != "SILENT":
        tts = gTTS(text=text, lang="en")
        filename = "voice.mp3"
        tts.save(filename)
        playsound.playsound(filename)


def listen():
    with suppress():
        r = sr.Recognizer()
        with sr.Microphone() as source:
            r.energy_threshold = 4000
            r.adjust_for_ambient_noise(source)
            audio = r.listen(source)
            said = ""
            try:
                said = r.recognize_google(audio)
            except Exception as e:
                pass
        return said

retry = 0


def ignore_words(command):
    for word in IGNORE_WORDS:
        commmand = command.replace(word, "")
    return " ".join(command.split())


def get_message_from_color_list(color_list):
    message = "Setting color to "
    colors = []
    for color in color_list:
        colors.append(color)

    return message + " and ".join(colors)


while RUN_SERVER:
    print("Listening")
    command = listen().lower()
    print("Command: ", command)
    if any(wake in command for wake in WAKE) > 0 or retry:
        signal = ""
        message = ""
        command = command.replace("hey thea", "").replace("hey there", "")
        is_vibe_command = command.count("vibe") > 0 or command.count("take me")

        if is_vibe_command + command.count("color") > 0:
            command = ignore_words(command)
            if is_vibe_command:
                found_colors = colorutils.find_vibes(command)
            else:
                found_colors = colorutils.find_colors(command)

            message = get_message_from_color_list(found_colors)
            rgbs = colorutils.to_rgb(found_colors)
            signal = "c" + str(len(rgbs))
            for rgb in rgbs:
                for v in rgb:
                    signal += "-" + str(v)
        # c2-0-23-2-34-51-0/
        elif command.count("brightness") > 0:
            brightness = int("".join(list(filter(str.isdigit, command))))
            signal = "b" + str(brightness)
            message = "Setting brightness to " + str(brightness) + " percent"
        # b12/
        elif command.count("off") + command.count("bye") + command.count("night") > 0:
            signal = "o"
            message = "Good night."
        # o/
        elif command.count("morning") > 0:
            signal = "m"
            message = "Good morning."
        # o/
        elif command.count("fast") > 0:
            signal = "f"
            message = "Transitioning faster."
        # f/
        elif command.count("slow") > 0:
            signal = "s"
            message = "Transitioning slower."
        # s/
        elif any(word in command for word in ["quiet", "silen", "stop", "shut"]):
            # mode = Mode.SILENT
            mode = "SILENT"
            continue
        elif command.count("speak") + command.count("talk"):
            mode = "DEFAULT"
            # mode = Mode.DEFAULT
            continue

        if signal:
            signal += "/"

            if mode != "SILENT":
                # if mode != Mode.SILENT:
                chime.success()
                speak(message)
            print("url", URL + signal)
            url = URL + signal

            while True:
                try:
                    response = requests.get(url=url, timeout=5)
                    break
                except:
                    continue
            continue

        retry += 1
        if retry >= 3:
            retry = 0
            if mode != "SILENT":
                chime.error()
        else:
            if retry == 1:
                speak("Sorry, I didn't catch that.")
            if retry == 2:
                speak("Could you say that again?")
def get_mikes():
    with suppress():
        l = sr.Microphone.list_microphone_names()
        print("printing microphone names:")
        print(l)


WAKE = "hey thea"

while True:
    print("Listening")
    command = listen().lower()
    if command.count(WAKE) > 0:
        speak("How can I help you?")
        command = listen()
