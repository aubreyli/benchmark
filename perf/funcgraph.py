"""
Gaussian HMM of stock data
--------------------------
This script trained Gaussian HMM on SH510300 price data.

Author: Aubrey Li

Ver: 2016-07-04
"""
from struct import *
import numpy as np
import sys,time,getopt
import os
import os.path
import math
from code import *
import multiprocessing

def obtain_log(logfile):
    count = 0
    lat = []
    ofile=open(logfile,'r')
    for line in ofile.readlines():
        ele = line.replace('+', '').replace('!','').strip().split()
        if len(ele) < 8:
            continue
        lat.append(float(ele[4]))
        count+=1
    ofile.close()
    return np.mean(lat),np.std(lat), count

if __name__ == "__main__":
    avg, std, count = obtain_log("funcgraph.log")
    #cpu_num = multiprocessing.cpu_count()
    #print(avg, std, count/float(cpu_num))
    print("Aubrey", round(avg, 2), round(std,2), int(count/10))
