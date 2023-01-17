#!/usr/bin/python3
import subprocess, time

subprocess.Popen(['stopwatch', '-forward'])
subprocess.Popen(['./minesweep.progs.x',  '60', '35', '40'])
time.sleep(1)
pid = subprocess.check_output(['xdotool', 'search', '--class', 'stopwatch'])
subprocess.Popen(['xdotool', 'windowmove', pid, '0', '0'])

