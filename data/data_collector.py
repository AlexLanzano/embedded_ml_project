#!/usr/bin/python

import serial
import json
import copy
import sys

def print_help():
    print("data_collector.py [OUTPUT_FILE]")

def main():
    if len(sys.argv) < 2:
        print_help()
        return

    output_file = sys.argv[1]
    running = True
    dataset = {'data': []}
    data = {'label': 'A',
            'points': []}
    points = []

    serial_device = serial.Serial("/dev/ttyUSB0", 115200);
    try:
        while True:
                print("Data count: {}".format(len(dataset['data'])))

            elif input_data == "RELEASED":
                new_points = copy.deepcopy(points)
                data['points'].append(new_points)
                points = []

            else:
                point = input_data.split(" ")
                point = [int(i) for i in point]
                points.append(point)
    except KeyboardInterrupt:
        with open(output_file, 'w') as json_file:
            json.dump(dataset, json_file)
        print("\nData written to {}".format(output_file))

main()
