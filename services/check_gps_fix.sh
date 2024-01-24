#!/bin/bash

while true; do
    if gpsmon -n | grep -q 'FIX'; then
        systemctl start update_clock_gps
        break
    fi
    sleep 1
done
