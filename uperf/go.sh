#!/bin/bash
# run uperf netperf workload with local host and TCP protocol

# test setup
echo "Starting follower uperf process..."
for i in 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240 96 144 192 240
do
	workload=netperf_$i.xml
	uperf -s &
	echo "Starting leader uperf process...$i threads, TCP protocol"
	uperf -m $workload -a -e -p
	echo "Clean up uperf workload after test"
	pkill uperf
	echo "uperf netperf workload test done!"
	sleep 10
done
