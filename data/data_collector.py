#!/usr/bin/python

import serial
import json
import copy

def main():
    running = True
    dataset = {'data': []}
    data = {'label': 'A',
            'points': []}

    serial_device = serial.Serial("/dev/ttyUSB0", 115200);
    try:
        while True:
            input_data = serial_device.read_until().decode('ascii').strip();

            if input_data == "DONE":
                new_data = copy.deepcopy(data)
                dataset['data'].append(new_data)
                data['points'] = []
                print("Data count: {}".format(len(dataset['data'])))
            else:
                point = input_data.split(" ")
                point = [int(i) for i in point]
                data['points'].append(point)
    except KeyboardInterrupt:
        with open('data.json', 'w') as json_file:
            json.dump(dataset, json_file)
        print("\nData written to data.json")

main()
