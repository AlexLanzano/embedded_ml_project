#!/usr/bin/python

import json
import sys
import copy

def print_help():
    print("data_converter.py [INPUT_FILE] [OUTPUT_FILE]")

def swap(a, b):
    return b, a

def draw_point(framebuffer, point):
    framebuffer[point[1]*30+point[0]] = 0
    return framebuffer

def draw_line(framebuffer, point1, point2):
    if (point1 == point2):
        return draw_point(framebuffer, point1)

    x0 = point1[0]
    y0 = point1[1]
    x1 = point2[0]
    y1 = point2[1]

    steep = abs(y1 - y0) > abs(x1 - x0)

    if (steep):
        x0, y0 = swap(x0, y0)
        x1, y1 = swap(x1, y1)

    if (x0 > x1):
        x0, x1 = swap(x0, x1)
        y0, y1 = swap(y0, y1)

    dx = x1 - x0
    dy = abs(y1 - y0)

    err = dx / 2
    if y0 < y1:
        ystep = 1;
    else:
        ystep = -1

    while x0 <= x1:
        if (steep):
            framebuffer = draw_point(framebuffer, [y0, x0])
        else:
            framebuffer = draw_point(framebuffer, [x0, y0])
        err -= dy
        if (err < 0):
            y0 += ystep
            err += dx
        x0 += 1

    return framebuffer

def map_point(point):
    p = []
    p.append(point[0] // 8)
    p.append(point[1] // 8)
    return p

def main():
    if len(sys.argv) < 3:
        print_help()
        return

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    output_dataset = {'data': []}
    output_data = {'label': 'A',
                   'framebuffer': []}

    with open(input_file, 'r') as json_file:
        input_dataset = json.load(json_file)
        for input_data in input_dataset['data']:
            output_data['label'] = input_data['label']
            output_data['framebuffer'] = [1] * (30*30)
            for points in input_data['points']:
                prev_point = None
                for point in points:
                    point = map_point(point)
                    if (prev_point == None):
                        output_data['framebuffer'] = draw_point(output_data['framebuffer'], point)
                    else:
                        output_data['framebuffer'] = draw_line(output_data['framebuffer'], prev_point, point);
                    prev_point = point
            new_data = copy.deepcopy(output_data)
            output_dataset['data'].append(new_data)

    with open(output_file, 'w') as json_file:
        json.dump(output_dataset, json_file)

main()
