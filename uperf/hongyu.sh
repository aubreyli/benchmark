#!/bin/bash
# run uperf netperf workload with local host and TCP protocol

# test setup
sudo echo
echo "Creating uperf cgroups"
sudo cgcreate -g cpu,cpuset:uperf

echo "Initializing cgroup cpuset:uperf.."

echo "Setting uperf:cpuset.cpus to 0-103"
#sudo cgset -r cpuset.cpus="0-3,18-21,72-75,90-93" uperf
sudo cgset -r cpuset.cpus="0-191" uperf

echo "Setting uperf:cpuset.mems to 0"
sudo cgset -r cpuset.mems="0" uperf

echo "Initializing cgroup cpuset:uperf done..."

echo "Setting core scheduling"
core_sched=$1
if [[ ${core_sched} = "no" ]]; then
        echo "core scheduling disabled"
        echo 0 | sudo tee "/sys/fs/cgroup/cpu/uperf/cpu.tag"
elif [[ ${core_sched} = "yes" ]]; then
        echo "core scheduling enabled"
        echo 1 | sudo tee "/sys/fs/cgroup/cpu/uperf/cpu.tag"
else
        echo "Wrong optargs provided"
        exit 1
fi


echo "Starting follower uperf process..."
sudo cgexec -g cpu,cpuset:uperf uperf -s &
echo "Starting leader uperf process...192 threads, TCP protocol"
sudo cgexec -g cpu,cpuset:uperf uperf -m netperf.xml -a -e -p > ./log/"core_sched_"$core_sched"_uperf_TCP_192".log

echo "Clean up uperf workload after test"
sudo pkill uperf

echo "uperf netperf workload test done!"

sudo cgset -r cpu.tag=0 uperf
sudo cgdelete -g cpu,cpuset:uperf
