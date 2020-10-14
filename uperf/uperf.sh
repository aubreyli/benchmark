#!/bin/bash
# run uperf netperf workload with local host and TCP protocol

# test setup
workload=netperf_$1.xml
echo "Starting follower uperf process..."
uperf -s &
echo "Starting leader uperf process...$1 threads, TCP protocol"
uperf -m $workload -a -e -p

echo "Clean up uperf workload after test"
pkill uperf
echo "uperf netperf workload test done!"
