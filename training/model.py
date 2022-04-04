import json
import random
import copy
import tensorflow as tf
from tensorflow import keras
import numpy as np
import matplotlib.pyplot as plt

def convert(framebuffer):
    fb = []
    for i in range(len(framebuffer)):
        if framebuffer[i] == 1:
            fb.append(0)
        elif framebuffer[i] == 0:
            fb.append(255)
        else:
            continue
    return fb

def get_image_data(dataset):
    image_data = []
    for data in dataset['data']:
        image_data.append(convert(data['framebuffer']))
    return image_data

def get_label_data(dataset):
    labels = []
    for data in dataset['data']:
        if (data['label'] == 'A'):
            labels.append(0)
        else:
            labels.append(1)
    return labels

def main():
    print("Opening Data Files...")
    train_A_data_file = open("../data/train_frame_data_A.json", "r")
    test_A_data_file = open("../data/test_frame_data_A.json", "r")
    train_B_data_file = open("../data/train_frame_data_B.json", "r")
    test_B_data_file = open("../data/test_frame_data_B.json", "r")

    print("Loading Data...")
    train_A_data = json.load(train_A_data_file)
    test_A_data = json.load(test_A_data_file)
    train_B_data = json.load(train_B_data_file)
    test_B_data = json.load(test_B_data_file)

    print("Processing Data")
    train_images = get_image_data(train_A_data) + get_image_data(train_B_data)
    test_images = get_image_data(test_A_data) + get_image_data(test_B_data)
    train_labels = get_label_data(train_A_data) + get_label_data(train_B_data)
    test_labels = get_label_data(test_A_data) + get_label_data(test_B_data)

    train_images = np.asarray(train_images)
    test_images = np.asarray(test_images)
    train_labels = np.asarray(train_labels)
    test_labels = np.asarray(test_labels)

    train_images = train_images / 255.0
    test_images = test_images / 255.0

    print("Creating Model...")
    model = keras.Sequential([
        keras.layers.Input(shape=(57600)),
        keras.layers.Dense(128, activation="relu"),
        keras.layers.Dense(2, activation="softmax")
    ])

    print("Compiling Model...")
    model.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=["accuracy"])

    print("Fitting Model...")
    model.fit(train_images, train_labels, epochs=20, shuffle=True)

    print("Evaluating Model...")
    test_loss, test_acc = model.evaluate(test_images, test_labels)
    print("\n Test Accuracy: ", test_acc)

main()
