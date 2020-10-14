#!/bin/bash
# run uperf netperf workload with local host and TCP protocol

# test setup
sudo echo
echo "Creating uperf cgroups"
sudo cgcreate -g cpu,cpuset:uperf

echo "Initializing cgroup cpuset:uperf.."

echo "Setting uperf:cpuset.cpus to 0-103"
sudo cgset -r cpuset.cpus="0-95" uperf

echo "Setting uperf:cpuset.mems to 0"
sudo cgset -r cpuset.mems="0" uperf

echo "Initializing cgroup cpuset:uperf done..."

echo "Starting follower uperf process..."
sudo cgexec -g cpu,cpuset:uperf uperf -s &
echo "Starting leader uperf process...192 threads, TCP protocol"
sudo cgexec -g cpu,cpuset:uperf uperf -m netperf.xml -a -e -p > ./log/"core_sched_"$core_sched"_uperf_TCP_192".log

echo "Clean up uperf workload after test"
sudo pkill uperf

echo "uperf netperf workload test done!"

sudo cgset -r cpu.tag=0 uperf
sudo cgdelete -g cpu,cpuset:uperf
