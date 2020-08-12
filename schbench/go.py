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

n_test = 3

def start_hackbench(n_groups):
	time_list = []
	cmd = "./schbench -c 3000000 -s 1000000 -t 1 -m 1 -r 1000"
	print(cmd)
	for i in range(n_test):
		result = os.popen(cmd)
		result.close()
##############################################
# main()
##############################################
if   __name__  ==  "__main__":
	for n in range(2, 17, 1):
		start_hackbench(n)
