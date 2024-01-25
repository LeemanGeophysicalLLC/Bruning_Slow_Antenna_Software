#!/usr/bin/env python3

import sys
import errno
import json
from os import system
from time import sleep

try:
    while True:
        # intended to have gpspipe -w piped to stdin
        line = sys.stdin.readline()
        if not line:
            # parent proc is dead, exit
            break
        # read json gpspipe data
        sentence = json.loads(line)
        # Filter to mode 2 (2d fix) and mode 3 (3d fix)
        if sentence['class'] == 'TPV':
            if sentence['mode'] == 2  or sentence['mode'] == 3:
                print('GPS fixed, updating clock')
                system('systemctl start update_clock_gps.service')
                while True:
                    if system('systemctl status update_clock_gps') != 0 and system('systemctl status adc_test_startup') != 0:
                        print('Starting data collection')
                        system('systemctl start adc_data_collect')
                        break
                    else:
                        sleep(1)
                break
except IOError as e:
    # this was stolen from stackoverflow and probably has a good reason to be here
    if e.errno == errno.EPIPE:
        pass
    else:
        raise e
