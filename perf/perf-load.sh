#result_dir=$1
result_dir=pt_tmp
echo "measure ...."
sudo perf-with-kcore record $result_dir -C0 -e intel_pt/cyc/k --filter="filter load_balance" -- sleep 10
echo "done. analysing ..."
sudo perf-with-kcore script $result_dir --itrace=cre --ns -Fcomm,time,sym,symoff,dso,addr,flags,callindent > intel_pt.log
python3 load_balance-funcduration.py
echo "done."
sudo rm -rf $result_dir
