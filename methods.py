import sys, math, datetime
import serial, string, os
import time
import serial
import io
from time import sleep
import time
from subprocess import check_output

def map(x, in_low, in_high, out_low, out_high):
    input_span = in_high - in_low
    output_span = out_high - out_low
    value_scaled = float(x - in_low) / float(input_span)
    return out_low + (value_scaled * output_span)

def play(song_num, song_dict):
    """
    Plays the song listed in song_num index of
    the song_dict
    """
    os.system('mpc crop')
    os.system('mpc add ' + song_dict[song_num])
    os.system('mpc play')

def set_volume(vol):
    os.system('mpc volume ' + str(vol))

def volume_is_zero():
    info = check_output(["mpc", "volume"]).strip("\n").split(" ")
    if info[len(info) - 1] == "0%":
        return True
    return False

def get_volume():
    info = check_output(["mpc", "volume"]).strip("\n").split(" ")
    return info[len(info) - 1][:-1]

def is_playing():
    if check_output(["mpc", "current"]) == '':
        return False
    return True
def stop():
    os.system('mpc clear')

def get_min_elapsed(start_time):
    return (time.time() - start_time) / 60.