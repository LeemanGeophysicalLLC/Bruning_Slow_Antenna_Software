from time import sleep
import numpy as np
import matplotlib.pyplot as plt
import struct
import datetime
import sys
from glob import glob
import scipy.signal as signal
import pandas as pd
import datetime
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import os
from matplotlib.ticker import FormatStrFormatter


time = pd.to_datetime("202306152033",format = "%Y%m%d%H%M")
paths = ["/Users/kelcy/Library/CloudStorage/OneDrive-TexasTechUniversity/jun 15/sensor 1",
         "/Users/kelcy/Library/CloudStorage/OneDrive-TexasTechUniversity/jun 15/sensor3",
         "/Users/kelcy/Library/CloudStorage/OneDrive-TexasTechUniversity/jun 15/sensor 8"]

fig, axs = plt.subplots(len(paths), figsize=(6.5, 2.5*len(paths)),sharex=False) #sharex=True,
# fig.suptitle(time)
fig.subplots_adjust(hspace=0.5)
# fig.tight_layout()
def convert_adc_to_decimal(value):
    modulo = 1 << 24
    max_value = (1 << 23) - 1
    if value > max_value:
        value -= modulo
    return value
SERIAL_SPEED = 2000000

def decode_data_packet(mp):
    # print(mp)
    result = dict()
    result['start_byte'] = struct.unpack('B', mp[0:1])[0]
    result['b1'] = struct.unpack('B', mp[1:2])[0]
    result['b2'] = struct.unpack('B', mp[2:3])[0]
    result['b3'] = struct.unpack('B', mp[3:4])[0]
    result['adc_pps_micros'] = struct.unpack('I', mp[4:8])[0]
    result['end_byte'] = struct.unpack('B', mp[8:9])[0]
    adc_hex = mp[1:4].hex()
    adc_ba = bytearray()
    adc_ba += mp[1:2]
    adc_ba += mp[2:3]
    adc_ba += mp[3:4]
    adc_ba += b'\x00'

    #print(mp[3:4])
    #print(adc_ba)
    adc_reading = struct.unpack('>i', adc_ba[:])[0]

    adc_reading = mp[1]
    adc_reading = (adc_reading << 8) | mp[2]
    adc_reading = (adc_reading << 8) | mp[3]
    adc_reading = convert_adc_to_decimal(adc_reading)


    result['adc_reading'] = adc_reading
    return result
def notch_sixty(s, fs):
    f0 = 60.0  # Frequency to be removed from signal (Hz)
    Q = 2.0  # Quality factor = center / 3dB bandwidth
    b, a = signal.iirnotch(f0, Q, fs)
    return signal.filtfilt(b, a, s)


for ind, path in enumerate(paths):
    files = sorted(glob(path+'/'+"*.raw"))
    dates = pd.to_datetime(sorted([os.path.splitext(os.path.basename(x))[0] for x in glob(path+'/'+"*.raw")]),format ="%Y%m%d%H%M%S_%f")
    sensor_num = files[0].split('/')
    min_time = time - timedelta(minutes = 4)
    max_time = time + timedelta(minutes = 4)
    # sensor_num = files.split('/')
    data_start_bytes = []
    data_packet_length = 8
    data_raw_packets = []
    data_packets = []
    inder = dates.get_loc(time, method = 'nearest')
    nearest_ind = [inder]
    if dates[inder] - timedelta(minutes = 3) >= min_time:
        nearest_ind.append(inder - 1)
    if dates[inder] + timedelta(minutes = 3) <= max_time:
        nearest_ind.append(inder + 1)

    time_arr = []
    print(nearest_ind)
    for _idx, fileind in enumerate(sorted(nearest_ind)):
        filename = files[fileind]
        data_start_bytes = []
        count = 0
        with open(filename, mode = 'rb') as file:
            ba = file.read()
            for i in range(len(ba) - data_packet_length):
                if (ba[i] == 190) and (ba[i+data_packet_length] == 239):
                    data_start_bytes.append(i)
                    count +=1
            this_packet_length = data_packet_length + 1
        
            data_raw_packets.extend([ba[sb:sb+this_packet_length] for sb in data_start_bytes[:-1]])
    
        
        time_offset = np.datetime64(datetime.strptime(filename[-25:-4], "%Y%m%d%H%M%S_%f"))
        timers = (time_offset + np.arange(count-1)*np.timedelta64(100, "us"))
        if _idx ==0:
            time_arr = timers
        else:
            time_arr = np.concatenate((time_arr ,timers))
    data_packets = [decode_data_packet(b) for b in data_raw_packets]
    

    starts = [dp['start_byte'] for dp in data_packets]
    adc_ready = [dp['adc_pps_micros'] for dp in data_packets]
    adc = [dp['adc_reading'] for dp in data_packets]
    end = [dp['end_byte'] for dp in data_packets]

    starts = np.array(starts)
    adc_ready = signal.medfilt(np.array(adc_ready), 7)
    adc = signal.medfilt(np.array(adc), 7)
    end = np.array(end)

    # print(adc.shape, adc.dtype)
    # delta_t_adc = (adc_ready[-1]-adc_ready[0])*1e-6
    # sample_rate = 1.0e6/np.median(np.ediff1d(adc_ready))
    # print(f"Elapsed time {delta_t_adc:6.3} s with sample rate {sample_rate:6.1f} Hz")

    # t = np.arange(adc.shape[0])/sample_rate

    bits_to_volts = (5/((2**24)-1))
    axs[ind].plot(time_arr, adc*bits_to_volts,label = sensor_num[7])#-adc_filt.mean())
# 	plt.gca().xaxis.set_major_formatter(FormatStrFormatter('%g'))
#     axs[ind].xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S.%f'))
#     plt.setp(axs[ind].get_xticklabels(), rotation = 15,visible=True)
#     axs[ind].set_xlim([datetime(2023,6,15,20,33,35), datetime(2023,6,15,20,33,55)])
# plt.legend(loc='upper right')

    # axs.set_title(str(sensor_num[7]))
#axs[1,1].set_xlabel('Time (s)')
# 	axs[ind].set_ylim([1.495,1.65])
#time_arr[0] + np.timedelta64(216, 's') + np.timedelta64(750,'ms'), time_arr[0] + np.timedelta64(218, 's')])#, 15:20:40)
# axs[ind].xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
# plt.setp(axs[ind].get_xticklabels(), rotation = 15)

# fig.subplots_adjust(vspace=0.5)
# plt.savefig("sensor_1.png",dpi = 300.0)
plt.show()


print('done with file')


# plt.savefig("sensor_1.png",dpi = 300.0)
