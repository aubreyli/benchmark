echo "Creating cgroups.."
sudo cgcreate -g cpu,cpuset:uperf

sudo cgset -r cpuset.cpus="0-191" uperf

echo "Setting cpuset.mems to 0..."
sudo cgset -r cpuset.mems=0 uperf

