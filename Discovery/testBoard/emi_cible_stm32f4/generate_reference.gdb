set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
#load macro.elf
#

hb main.c:66
commands 1
	i r
	x/800xw 0x20000200
	c
end

hb main.c:317
commands 2
	i r
	x/800xw 0x20000200
	disass
end

hb stm32f4xx_it.c:63
commands 3
	i r
	x/800xw 0x20000200
end

c
set logging off
quit








