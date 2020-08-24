sudo trace-cmd record -p function_graph -M 1 -l load_balance -- sleep 10
sudo trace-cmd report > funcgraph.log
python3 funcgraph.py
sudo rm trace.dat
sudo rm funcgraph.log
