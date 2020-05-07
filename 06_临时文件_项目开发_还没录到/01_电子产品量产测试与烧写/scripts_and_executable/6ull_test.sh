#!/bin/sh

DEBUG_DEV=/dev/null

# 1.烧写系统
update_emmc() {
	echo clean > /tmp/rate.out

	echo 0 > /sys/block/mmcblk1boot0/force_ro     
	
	dd if=/root/u-boot-dtb.imx of=/dev/mmcblk1boot0 bs=512 seek=2 
	dd if=/root/emmc.img of=/dev/mmcblk1 bs=512K > /tmp/rate.out 2>&1 &
	while :
	do
		rate=$(cat /tmp/rate.out | awk 'END {print}')
		echo $rate | grep -q "%"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "burn percent $rate"
			echo $rate > $DEBUG_DEV
		fi
		
		echo $rate | grep -q "copied"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "burn percent 100"
			echo "100%" > $DEBUG_DEV
			break
		fi
		sleep 1
	done
	
	echo 1 > /sys/block/mmcblk1boot0/force_ro  
	detect_dev 127.0.0.1 "burn ok"
	echo "burn ok" > $DEBUG_DEV
}

# 2.测试模块

# 2.1 无需操作
# 2.1.1 LED(观察底板，LED闪烁即可)
# 2.1.2 I2C接口的AP3216C
test_ap3216c() {
	while :
	do
		i2cdetect -r -y 0 | grep -q "1e"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "ap3216c ok"
			echo "ap3216c ok" > $DEBUG_DEV
		else
			echo "ap3216c err" > $DEBUG_DEV
		fi
		sleep 1
	done		
}

# 2.1.3 icm20608
test_icm20608() {
	modprobe  inv_mpu6050_spi
	while :
	do
		cat  /sys/bus/iio/devices/iio:device1/name | grep -q "icm20608"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "icm20608 ok"
			echo "icm20608 ok" > $DEBUG_DEV
		fi
		sleep 1
	done		
}

# 2.2 需要操作
# 2.2.1 两个按键
test_key1() {
	while :
	do
		hexdump /dev/input/event2 -n 15 | grep -q "0001 0002 0001 0000"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "key1 ok"
			echo "key1 ok" > $DEBUG_DEV
		fi
		sleep 0.1
	done
}
test_key2() {
	while :
	do
		hexdump /dev/input/event2 -n 15 | grep -q "0001 0003 0001 0000"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "key2 ok"
			echo "key2 ok" > $DEBUG_DEV
		fi
		sleep 0.1
	done
}

test_key() {
	test_key1 &
	test_key2 &
}

# 2.3 需要连接

# 2.3.1 can 485
test_can_rs485() {
	echo 128 > /sys/class/gpio/export
	echo out > /sys/class/gpio/gpio128/direction
	ip link set can0 down
	ip link set can0 type can bitrate 500000
	ip link set can0 up
	
	candump can0  > /tmp/rs485tocan.out &
	rs485_read -d /dev/ttymxc2 -b 9600 > /tmp/cantors485.out &
	
	while :
	do
		echo 1 > /sys/class/gpio/gpio128/value
		echo 12345 > /dev/ttymxc2
		grep -q "31 32 33 34 35 0A" /tmp/rs485tocan.out
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "RS485toCAN ok"
			echo "rs485 to can ok" > $DEBUG_DEV
			echo clean > /tmp/rs485tocan.out
		fi
		sleep 0.5
		
		touch /tmp/cantors485.out
		echo 0 > /sys/class/gpio/gpio128/value
		cansend can0 123#AABBCC
		grep -q "0xaa 0xbb 0xcc" /tmp/cantors485.out
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "CANtoRS485 ok"
			echo "can to rs485 ok" > $DEBUG_DEV
			echo clean > /tmp/cantors485.out
		fi
		
		sleep 2
	done
}

# 2.3.2 networkx2
test_network() {
	ip netns add ns_server > $DEBUG_DEV
	ip netns add ns_client > $DEBUG_DEV

	ip link set eth0 netns ns_server > $DEBUG_DEV
	ip netns exec ns_server ip addr add dev eth0 192.168.1.1/24  > $DEBUG_DEV
	ip netns exec ns_server ip link set dev eth0 up  > $DEBUG_DEV
	ip link set eth1 netns ns_client  > $DEBUG_DEV
	ip netns exec ns_client ip addr add dev eth1 192.168.1.2/24  > $DEBUG_DEV
	ip netns exec ns_client ip link set dev eth1 up  > $DEBUG_DEV

	while :
	do
		ip netns exec ns_client ping 192.168.1.1 -c 4 | grep -q "4 received"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "net1 ok"
			echo "net0 ok" > $DEBUG_DEV
		fi
		sleep 0.5
		
		ip netns exec ns_server ping 192.168.1.2 -c 4  | grep -q "4 received"
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "net0 ok"
			echo "net1 ok" > $DEBUG_DEV
		fi
		sleep 0.5
	done
}

# 2.3.3 test_usb_otg_seria_wifi_4g
test_usb_otg_seria_wifi_4g() {
	modprobe g_ether
	modprobe cp210x

	while :
	do
		lsusb | grep -q "0525:a4a2"  
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "otg_device ok"
			detect_dev 127.0.0.1 "usb ok"
			echo "otg_device ok" > $DEBUG_DEV
			echo "usb ok" > $DEBUG_DEV
		fi
		sleep 0.1
		
		lsusb | grep -q "10c4:ea60"  
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "otg_host ok"
			echo "otg_host ok" > $DEBUG_DEV
			
			serialName=$(ls /sys/bus/usb-serial/drivers/cp210x | grep "ttyUSB")
			serial_test /dev/$serialName | grep -q "Serial ok"  
			if [ $? -eq 0 ]
			then
				detect_dev 127.0.0.1 "serial ok"
				echo "serial ok" > $DEBUG_DEV
			fi
		fi
		sleep 0.1
		
		lsusb | grep -q "0bda:b720"  
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "wifi ok"
			echo "wifi ok" > $DEBUG_DEV
		fi
		sleep 0.1
		
		lsusb | grep -q "2c7c:0125"  
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "4G ok"
			echo "4G ok" > $DEBUG_DEV
		fi
		sleep 0.1
		
		sleep 1
	done
}

# 2.3.4 test_alsa
test_speaker() {
	while :
	do
		speaker-test -t wav -c 2 -D plughw:0,0  > $DEBUG_DEV
		detect_dev 127.0.0.1 "speaker play"     > $DEBUG_DEV
	done
}

test_record() {
	echo clean > record.out
	while :
	do
		arecord -vvv  -d 5 test.wav > record.out 2>&1 
		grep -q "[3-7][0-9]%" record.out
		if [ $? -eq 0 ]
		then
			detect_dev 127.0.0.1 "record ok"
			echo "record ok" > $DEBUG_DEV
		fi
		sleep 0.1
	done
}

test_alsa() {
	test_speaker &
	test_record &
}


echo ------- start_update ----- > $DEBUG_DEV
update_emmc &

echo ------- start_test ----- > $DEBUG_DEV
test_ap3216c   &
test_icm20608  &
test_key  &
test_can_rs485 &
test_network &
test_usb_otg_seria_wifi_4g &
test_alsa &


# rm /tmp/rs485tocan.out
# rm /tmp/cantors485.out
# rm /tmp/rate.out
