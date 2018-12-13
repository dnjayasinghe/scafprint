#!/bin/sh

x=1
while [ $x -le 10000 ]
do
	JLinkGDBServer -select usb -if jtag -speed 25000 -device stm32f407vg> test &
	PROC=$!
	while [ -d /proc/$PROC ];do
        echo -n '.' ; sleep 0.5
	done
	x=$(( $x + 1 ))
done

