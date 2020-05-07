#!/bin/sh
status=$1
if [ "$status" = "processing" ]
then
wakeup_count=$(cat /sys/power/wakeup_count)
echo standby > /sys/power/state

wakeup_count_new=$(cat /sys/power/wakeup_count)
while [ $wakeup_count_new -eq $wakeup_count ]
do
	wakeup_count_new=$(cat /sys/power/wakeup_count)
done

count=0
while [ $count -le 1 ]
do
	detect_dev 127.0.0.1 "ALL ok" > /dev/null 2>&1	
	sleep 1
        let count++
done

fi


