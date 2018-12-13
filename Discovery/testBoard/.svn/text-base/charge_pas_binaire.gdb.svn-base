#set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
load IO_Toggle.elf
hb main.c:317
commands 1
	i r
	x/800xw 0x20000200
end

hb stm32f4xx_it.c:63
commands 2
	i r
	x/800xw 0x20000200
end

c
set logging off
quit








