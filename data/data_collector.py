#!/usr/bin/python

import serial
import json
import copy
import sys

def print_help():
    print("data_collector.py [DATA_LABEL] [OUTPUT_FILE]")

def main():
    if len(sys.argv) < 3:
        print_help()
        return

    data_label = sys.argv[1]
    output_file = sys.argv[2]
    running = True
    dataset = {'data': []}
    data = {'label': data_label,
            'points': []}
    points = []

    serial_device = serial.Serial("/dev/ttyUSB0", 115200);
    try:
        while True:
            input_data = serial_device.read_until().decode('ascii').strip();

            if input_data == "Done":
                new_data = copy.deepcopy(data)
                dataset['data'].append(new_data)
                data['points'] = []
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
