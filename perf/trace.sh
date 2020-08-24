sudo trace-cmd record -p function_graph -M 1 -l select_task_rq_fair -- sleep 10
sudo trace-cmd report > funcgraph.log
python3 funcgraph.py
sudo rm trace.dat
sudo rm funcgraph.log
