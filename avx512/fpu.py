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

def offline_cpu(cpu_id):
	cmd = "echo 0 > /sys/devices/system/cpu/cpu"+str(cpu_id)+"/online"
	os.system(cmd)

##############################################
# main()
##############################################
if   __name__  ==  "__main__":
	for cpu in range(10, 26):
		offline_cpu(cpu)
	for cpu in range(10 + 52, 26 + 52):
		offline_cpu(cpu)
	for cpu in range(26, 52):
		offline_cpu(cpu)
	for cpu in range(26 + 52, 52 + 52):
		offline_cpu(cpu)
