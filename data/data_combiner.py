#!/usr/bin/python

import json
import sys
import copy

def print_help():
    print("data_combiner.py [INPUT_DATA]... [OUTPUT_DATA]")

def main():
    if len(sys.argv) < 3:
        print_help()
        return

    input_count = len(sys.argv) - 1
    output_file = sys.argv[-1]
    dataset = {'data': []}

    for i in range(1, input_count):
        with open(sys.argv[i], 'r') as json_file:
            input_dataset = json.load(json_file)
            for input_data in input_dataset['data']:
                copy_data = copy.deepcopy(input_data)
                dataset['data'].append(copy_data)

    with open(output_file, 'w') as json_file:
        json.dump(dataset, json_file)

main()
