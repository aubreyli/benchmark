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

def bind_cpu(cpu, pid):
	cmd = "taskset -pc "+str(cpu)+" "+str(pid)
	os.system(cmd)

##############################################
# main()
##############################################
if   __name__  ==  "__main__":
	cpu = int(sys.argv[1])
	pid = int(sys.argv[2])
	num = int(sys.argv[3])

	for i in range(0, num):
		bind_cpu(cpu+i, pid+i);
