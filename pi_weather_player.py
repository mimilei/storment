import serial
import os
from random import randint
import time
from methods import *

ser = serial.Serial('/dev/ttyACM0', 9600)
num_sounds = 3
sound_arr = ["thunder.mp3", "thunder2.mp3", "thunder3.mp3"]
start_time = None
single_queue = False # has a song already been queued that isn't playing?
curr_vol = 0

# set up mpc
os.system('mpc update')
os.system('mpc clear')
os.system('mpc -p 6600')
os.system('mpc volume 0')
os.system('mpc crossfade 2')
os.system('mpc repeat on')
os.system('mpc random on')
for s in sound_arr:
    os.system('mpc add ' + str(s))
os.system('mpc play')

try: 
    while True:
        from_arduino = ser.readline()[:-2]
        print("from_arduino = " + str(from_arduino))
        if from_arduino == "Open to closed!":
            start_time = time.time() # gives time in seconds

        if from_arduino == "DOOR CLOSED":
            if start_time == None:
                start_time = time.time()
            # min_elapsed = get_min_elapsed(start_time)
            # curr_vol = int(map(min_elapsed, 0, 15, 50, 100))
            # set_volume(curr_vol)
            set_volume(80)
            curr_vol = 80
        elif from_arduino == "DOOR OPEN": # door open
            # if not volume_is_zero():
            if curr_vol != 0:
                countdown = int(get_volume())
                while countdown > 0:
                    set_volume(countdown)
                    countdown = countdown - 5
                set_volume(0)
                curr_vol = 0
            
except KeyboardInterrupt:
    os.system('mpc clear')