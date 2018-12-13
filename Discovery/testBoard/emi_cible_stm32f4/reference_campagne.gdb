set verbose off
set pagination off
set confirm off			
set logging on
target extended-remote localhost:2331
#load build/stm32f4_usb_cdc_mini_ecsm.elf
hb main
commands 1
#        c
end
c
hb main.c:355
commands 2
#        print K
#	x/12xw 0x20001a18
#	print 1
	set {int}0x20001ac4=0x34353161
	set {int}0x20001ac0=0x31663435
	set {int}0x20001abc=0x61636431
	set {int}0x20001ab8=0x61623662
	set {int}0x20001ab4=0x30623961
	set {int}0x20001ab0=0x30653338
	set {int}0x20001aac=0x39396637
	set {int}0x20001aa8=0x35636539
	set {int}0x20001aa4=0x31633266
	set {int}0x20001aa0=0x32393532
	set {int}0x20001a9c=0x64373462
	set {int}0x20001a98=0x33396464
        print K
#	x/12xw 0x20001a18
#	set {int}0x20001A0C=0x37303030
#	set {int}0x20001A08=0x30303030
#	set {int}0x20001A04=0x30303030
#	set {int}0x20001A00=0x30303030
#	set {int}0x200019fc=0x30303030
#	set {int}0x200019f8=0x30303030
#	set {int}0x200019f4=0x30303030
#	set {int}0x200019f0=0x30303030
#	set {int}0x200019eC=0x30303030
#	set {int}0x200019e8=0x30303030
#	set {int}0x200019e4=0x30303030
#	set {int}0x200019e0=0x30303030
#        c
end
c
hb main.c:404
commands 3
#	i r
#	print 1
#	x/3xw 0x2001FF18
#	x/3xw 0x2001FF24
#	x/3xw 0x2001FF30
#        print "BUFFER :"
#        x/30xw BUFFER
	print "x"
	x/6xw 0x20000a14
	print "y"
	x/6xw 0x20000a3c
	print "z"
	x/1xw 0x20000a64
	print "flag"
	x/1xw 0x2001fef0
#	print "x"
#	x/6xw 0x20001c48
#	print "y"
#	x/6xw 0x20001c78
#	print "z"
#	x/1xw 0x20001d18
#	print "flag"
#	x/1xw 0x2001fef0
	set logging off
	quit
end
hb stm32f4xx_it.c:63
commands 4
	#i r
	#x/800xw 0x20000200
	set logging off
	quit
end
c
set logging off
quit
