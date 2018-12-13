
#set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
load build/stm32f4_usb_cdc_mini_ecsm.elf
#hb main
#commands 1
#	i r
#end
#c
#hb main.c:388
#commands 2
#	i r
#	x/800xw 0x20000200
#end

#run
#c
set logging off
quit

