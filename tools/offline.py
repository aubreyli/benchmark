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

def cpu_offline(cpu_num):
	cmd = "echo 0 > /sys/devices/system/cpu/cpu"+str(cpu_num)+"/online"
	#print(cmd)
	os.system(cmd)

##############################################
# main()
##############################################
if   __name__  ==  "__main__":
    start = int(sys.argv[1])
    end = int(sys.argv[2])
    for cpu_num in range(start, end+1):
	cpu_offline(cpu_num)
