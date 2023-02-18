import matplotlib.colors as mcolors
import numpy as np
from numpy.linalg import norm
import json


embeddings = {}
with open("models/glove.6B.100d.txt", "r") as f:
    for line in f:
        values = line.split()
        word = values[0]
        vector = np.asarray(values[1:], "float32")
        embeddings[word] = vector


def similarity(word1, word2):
    x, y = embeddings[word1], embeddings[word2]
    return np.dot(x, y) / norm(x) / norm(y)


colors = mcolors.CSS4_COLORS
colors_set = set(mcolors.CSS4_COLORS.keys())

color_from_code = {v: k for k, v in colors.items()}

with open("colors.json", "r") as openfile:
    color_names_with_spaces = json.load(openfile)


def to_rgb(color_list):
    return list(
        map(
            lambda color: list(
                map(lambda x: int(255 * x), mcolors.to_rgb(colors[color]))
            ),
            color_list,
        )
    )


def find_colors(command):
    words = command.split(" ")

    L = len(words)
    found_colors = []
    while L > 0:
        for i in range(0, len(words) - L + 1):
            if "".join(words[i : i + L]) in colors_set:
                found_colors.append("".join(words[i : i + L]))
                words = words[:i] + words[i + L :]
                L = min(len(words), L)
        L -= 1

    return found_colors


class SortedTriplet:
    def __init__(self):
        self.triplet = []

    def add(self, element, value):

        for i in range(len(self.triplet)):
            if value > self.triplet[i]["val"]:
                self.triplet = (
                    self.triplet[:i]
                    + [{"node": element, "val": value}]
                    + self.triplet[i:2]
                )
                return

        if len(self.triplet) < 3:
            self.triplet += [{"node": element, "val": value}]

    def get(self):
        output = []
        for d in self.triplet:
            output.append(d["node"])
        return output


def find_vibes(command):
    words = command.split(" ")
    found_colors = SortedTriplet()
    for word in words:
        for c in color_names_with_spaces:
            cwords = c.split(" ")
            cvals = []
            for cword in cwords:
                cvals.append(similarity(cword, word))

            found_colors.add(c.replace(" ", ""), sum(cvals) / len(cvals))
    return found_colors.get()
