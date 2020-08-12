#sudo mkdir -p /sys/fs/cgroup/cpu/schbench_root
#sudo cgexec -g cpu:schbench_root ./schbench -c 3000000 -s 1000000 -t 4 -m 1 -r 1000
./schbench -c 1000000 -s 10 -t 8 -m 1 -r 1000
