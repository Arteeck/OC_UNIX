#!/bin/bash
rm -f somefile.lck stats
make -s task
if [[ $1 == "-b" ]]
then
	./task &
	rm -f somefile.lck
	wait
	exit
fi

count=10
pids=()
for (( i=0; i<$count; i++ ))
do
	./task &
	pids+=($!)
done
sleep 5m
completedTasks=0
for pid in ${pids[@]}
do
	kill -SIGINT $pid
	if [[ $? -eq 0 ]]
	then
		((completedTasks++))
	fi
done

resultFileName="results.txt"
echo "Проверяем, что все задачи успешно завершились по сигналу SIGINT" >"$resultFileName"
echo "Ожидалось:" $count >>"$resultFileName"
echo "По факту:" $completedTasks >>"$resultFileName"

echo "Проверяем, что в файле статистики находится ожидаемое количество строчек" >>"$resultFileName"
echo "Ожидалось:" $count >>"$resultFileName"
echo "По факту:" $(wc -l stats | awk '{ print $1 }') >>"$resultFileName"

echo "Проверяем, что каждая задача захватила блокировку хотя бы раз" >>"$resultFileName"
echo "Ожидалось: все числа > 0" >>"$resultFileName"
echo "По факту:" $(cut -d: -f2 stats | xargs) >>"$resultFileName"
