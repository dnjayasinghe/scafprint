#!/bin/bash
# Sources

SRCS = main.c mini-gmp.c

# USB
SRCS += stm32f4xx_it.c system_stm32f4xx.c syscalls.c usbd_usr.c usbd_cdc_vcp.c usbd_desc.c usb_bsp.c

# Project name

PROJ_NAME=stm32f4_usb_cdc_mini_ecsm
OUTPATH=build
EXECUTABLE=$(PROJ_NAME).elf
CREF_FILE=reference_campagne.gdb
AREF_FILE=reference_analyse.gdb

CONFC= --quiet --batch --command=reference_campagne.gdb
CONFA= --quiet --batch --command=reference_analyse.gdb



###################################################

# Check for valid float argument
# NOTE that you have to run make clean after
# changing these as hardfloat and softfloat are not
# binary compatible
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
override FLOAT_TYPE = hard
#override FLOAT_TYPE = soft
endif
endif

###################################################

BINPATH=~/sat/bin
CC= arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size
#STLINK_HOME=.././stlink/


# Choose debug or release...
CFLAGS = -g -O0 -fomit-frame-pointer           # Normal
#CFLAGS = -ggdb -O1 -fomit-frame-pointer    # RSW - for GDB debugging, disable optimizer


CFLAGS += -std=gnu99 -Wall -Tstm32_flash.ld
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4 -I/home/zak/articles_2014/2015/15_XXX_FIAGCM/ECDSA/mini-ecsm/STM32F4_USB_CDC/src/

ifeq ($(FLOAT_TYPE), hard)
CFLAGS += -fsingle-precision-constant 

CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
else
CFLAGS += -msoft-float
endif

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

# Includes
CFLAGS += -Iinc -Ilib/Core/cmsis -Ilib/Core/stm32
CFLAGS += -Ilib/Conf

# Library paths
LIBPATHS = -Llib/StdPeriph -Llib/USB_Device/Core
LIBPATHS += -Llib/USB_Device/Class/cdc -Llib/USB_OTG

# Libraries to link
LIBS = -lm -lstdperiph -lusbdevcore -lusbdevcdc -lusbcore

# Extra includes
CFLAGS += -Ilib/StdPeriph/inc
CFLAGS += -Ilib/USB_OTG/inc
CFLAGS += -Ilib/USB_Device/Core/inc
CFLAGS += -Ilib/USB_Device/Class/cdc/inc

# add startup file to build
SRCS += lib/startup_stm32f4xx.s

OBJS = $(SRCS:.c=.o)

###################################################

TEMP=10
export TEMP
CPPFLAGS?= -DTEMPO=$(TEMP)

all: lib proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

lib:
	$(MAKE) -C lib FLOAT_TYPE=$(FLOAT_TYPE)

proj: 	$(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin
	$(MAKE) clean -C lib # Remove this line if you don't want to clean the libs as well

reference_campagne:
	#JLinkGDBServer -select usb -if jtag -speed 10000 -device stm32f407vg&
	./configure_generate_reference.pl "" "reference_campagne.gdb" "$(OUTPATH)/$(EXECUTABLE)" "303030303030303030303030303030303030303030303030303030303030303030303030303030306666666666666662" "646339356532633063313533323330616332623535386465626636363030316264336562336162333439346334633561" "0x20001c48" "0x20001c78" "0x20001d18" "0x2001fef0"
	rm -rf gdb.txt .gdb.txt
	arm-none-eabi-gdb $(OUTPATH)/$(EXECUTABLE) $(CONFC)
	mv gdb.txt output1.ref
	#pkill -9 JLinkGDBServer
refererence_analyse:
	#JLinkGDBServer -select usb -if jtag -speed 10000 -device stm32f407vg&
	./configure_generate_reference_init.pl "" "reference_analyse.gdb"
	rm -rf gdb.txt .gdb.txt
	arm-none-eabi-gdb $(OUTPATH)/$(EXECUTABLE) $(CONFA)
	mv gdb.txt reference_macroX.log
	#pkill -9 JLinkGDBServer
test:
	#pkill -9 JLkGDB.sh
	#JLinkGDBServer -select usb -if jtag -speed 25000 -device stm32f407vg&
	./Inject_minimal.pl "1000" "reference_campagne.gdb" "$(OUTPATH)/$(EXECUTABLE)" "$(shell date +%Y-%m-%d_%H-%M)"
	#pkill -9 JLinkGDBServer
pgm:
	#pkill -9 JLkGDB.sh
	#JLinkGDBServer -select usb -if jtag -speed 25000 -device stm32f407vg&
	./configure.pl "pgm" "cfg.gdb" "$(OUTPATH)/$(EXECUTABLE)"
	#arm-none-eabi-gdb macro.elf --quiet --batch --command=cfg.gdb
	#pkill -9 JLinkGDBServer
	#./JLkGDB.sh > JLKGDB.log&
reset:
	./kill.sh
	JLinkExe poweron.jlk -device STM32F407VG -if jtag -speed 25000 
	#./JLkGDB.sh > JLKGDB.log&
poweroff:
	./kill.sh
	JLinkExe poweroff.jlk -device STM32F407VG -if jtag -speed 25000

erase:
	./kill.sh
	JLinkExe erase.jlk -device STM32F407VG -if jtag -speed 25000

.PHONY: all lib proj clean test pgm reference_campagne reference_analyse reset poweroff erase
