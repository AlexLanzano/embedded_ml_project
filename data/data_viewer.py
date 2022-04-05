#!/usr/bin/python

import sys
import json
import pygame
from pygame.locals import *

def print_help():
    print("data_viewer.py [INPUT_FILE] [DATA_INDEX]")

def pixel_to_color(pixel):
    if (pixel == 1):
        return (255, 255, 255)
    else:
        return (255, 0, 0)


def render_framebuffer(surface, framebuffer):
    x = 0
    y = 0
    rect = [0, 0, 3, 3]
    for pixel in framebuffer:
        color = pixel_to_color(pixel)
        rect[0] = x * 3
        rect[1] = y * 3
        pygame.draw.rect(surface, color, rect)
        if (x < 29):
            x += 1
        else:
            x = 0
            y += 1
    pygame.display.flip()

def main():
    if len(sys.argv) < 3:
        print_help()
        return

    pygame.init()
    surface = pygame.display.set_mode((720, 720), 0, 32)
    input_file = sys.argv[1]
    data_index = int(sys.argv[2])

    with open(input_file, 'r') as json_file:
        dataset = json.load(json_file)
        if (len(dataset['data']) <= data_index):
            print("Invalid index")
            return
        data = dataset['data'][data_index]
        render_framebuffer(surface, data['framebuffer'])

    while True:
        a = 1

main()
