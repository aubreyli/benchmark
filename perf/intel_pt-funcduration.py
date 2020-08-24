#!/usr/bin/python

###############################################################
# This program is used to disable c-state thru /sys interface
################################################################


__author__ = "Aubrey Li (aubreylee@gmail.com)"
__version__ = "$Revision: 1.0 $"
__date__ = "$Date: 2017/05/04 10:10:10 $"
__copyright__ = "Copyright (c) 2012 Aubrey Li"
__license__ = "Python"

import sys
import os
import re
import time
import numpy as np

skip_header = 3
call_step = 6
return_index = call_step - 1
addr_entry = "ffffffff810ca6e0"
addr_return = "ffffffff810c3c31"

def function_duration(filename):
    fd = open(filename, 'r')
    count = 0
    lat = []
    for line in fd.readlines():
        item = line.replace('.','')
        item = item.replace(':','').strip().split()
        if addr_entry in item:
            ts_entry = float(item[1])
        if addr_return in line and ts_entry != 0:
            ts_latency = float(item[1]) - ts_entry
            if (ts_latency < 0):
                print("Error...\n")
            ts_entry = 0
            lat.append(ts_latency)
            count = count + 1
    fd.close()
    return np.mean(lat),np.std(lat), count

##############################################
# main()
##############################################
if   __name__  ==  "__main__":
    avg, std, count = function_duration("intel_pt.log")
    print("Aubrey", round(avg/1000, 2), round(std/1000, 2), int(count/10))
