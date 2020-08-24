for i in 1 2 3
do
	./perf.sh
	sleep 30
	./trace.sh
	sleep 30
done

for i in 1 2 3
do
	./perf-load.sh
	sleep 30
	./trace-load.sh
	sleep 30
done
