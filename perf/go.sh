for i in 1 2 3
do
	./perf.sh
	sleep 30
	./trace.sh
	sleep 30
done
