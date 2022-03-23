import tensorflow as tf
from tensorflow import keras
import numpy as np
import matplotlib.pyplot as plt

# (train_images, train_labels), (test_images, test_labels) = load_data()

(train_images, train_labels), (test_images, test_labels) = keras.datasets.mnist.load_data()
train_images = train_images / 255
test_images = test_images / 255

model = keras.Sequential([
    keras.layers.Input(shape=(28,28)),
    keras.layers.Flatten(),
    keras.layers.Dense(64, activation="relu"),
    keras.layers.Dense(10, activation="softmax")
])

model.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=["accuracy"])
model.fit(train_images, train_labels, epochs=5)

test_loss, test_acc = model.evaluate(test_images, test_labels)
print("\n Test Accuracy: ", test_acc)
